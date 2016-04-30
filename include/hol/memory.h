#ifndef HOL_MEMORY_H
#define HOL_MEMORY_H
//
// Copyright (c) 2016 Galik <galik.bool@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include <memory>

namespace hol {

// Casey's default-init allocator
// http://stackoverflow.com/a/21028912/3807729

// Allocator adaptor that interposes construct() calls to
// convert value initialization into default initialization.
template <typename T, typename A=std::allocator<T>>
class default_init_allocator : public A {
  typedef std::allocator_traits<A> a_t;
public:
  template <typename U> struct rebind {
    using other =
      default_init_allocator<
        U, typename a_t::template rebind_alloc<U>
      >;
  };

  using A::A;

  template <typename U>
  void construct(U* ptr)
    noexcept(std::is_nothrow_default_constructible<U>::value) {
    ::new(static_cast<void*>(ptr)) U;
  }
  template <typename U, typename...Args>
  void construct(U* ptr, Args&&... args) {
    a_t::construct(static_cast<A&>(*this),
                   ptr, std::forward<Args>(args)...);
  }
};

// end

template<typename ElementType>
class access_ptr
{
public:
	using element_type = ElementType;

private:
	element_type* p;

public:
	access_ptr(): p(nullptr) {}
	access_ptr(element_type* p): p(p) {}
	access_ptr(const access_ptr& ptr): p(ptr.p) {}
	access_ptr(std::unique_ptr<element_type>& ptr): p(ptr.get()) {}
	access_ptr(std::shared_ptr<element_type>& ptr): p(ptr.get()) {}

	access_ptr& operator=(access_ptr ptr)
	{
		p = ptr.p;
		return *this;
	}

	constexpr element_type* release() noexcept { element_type* p = this->p; this->p = nullptr; return p; }
	constexpr void reset(element_type* p = nullptr) noexcept { this->p = p; }
	constexpr void swap(access_ptr& other) noexcept { std::swap(p, other.p); }
	constexpr element_type* get() const noexcept { return p; }
	constexpr explicit operator bool() const noexcept { return p; }

	friend bool operator==(access_ptr a, decltype(nullptr)) { return a.p; }
	friend bool operator==(decltype(nullptr), access_ptr a) { return a.p; }

	constexpr std::add_lvalue_reference_t<element_type> operator*() const { return *p; }
	constexpr element_type* operator->() const { return p; }

//	element_type* operator->() { return p; }
//	element_type const* operator->() const { return p; }
//	element_type& operator*() { return *p; }
//	element_type const& operator*() const { return *p; }
};

template<typename ElementType>
access_ptr<ElementType> gain_access(std::shared_ptr<ElementType>& ptr)
{
	return access_ptr<ElementType>(ptr);
}

template<typename ElementType>
access_ptr<ElementType> gain_access(std::unique_ptr<ElementType>& ptr)
{
	return access_ptr<ElementType>(ptr);
}

} // hol

template<class E1, class E2>
bool operator==(const hol::access_ptr<E1>& ptr1, const hol::access_ptr<E2>& ptr2)
{
	return ptr1.get() == ptr2.get();
}

template<class E1, class E2>
bool operator!=(const hol::access_ptr<E1>& ptr1, const hol::access_ptr<E2>& ptr2)
{
	return ptr1.get() != ptr2.get();
}

template<class E>
bool operator==(const hol::access_ptr<E>& ptr, std::nullptr_t)
{
	return !bool(ptr);
}

template<class E>
bool operator==(std::nullptr_t, const hol::access_ptr<E>& ptr)
{
	return !bool(ptr);
}

template<class E>
bool operator!=(const hol::access_ptr<E>& ptr, std::nullptr_t)
{
	return bool(ptr);
}

template<class E>
bool operator!=(std::nullptr_t, const hol::access_ptr<E>& ptr)
{
	return bool(ptr);
}

template<class E1, class E2>
bool operator<(const hol::access_ptr<E1>& ptr1, const hol::access_ptr<E2>& ptr2)
{
	return ptr1.get() < ptr2.get();
}

template<class E1, class E2>
bool operator>(const hol::access_ptr<E1>& ptr1, const hol::access_ptr<E2>& ptr2)
{
	return ptr1.get() > ptr2.get();
}

template<class E1, class E2>
bool operator<=(const hol::access_ptr<E1>& ptr1, const hol::access_ptr<E2>& ptr2)
{
	return ptr1.get() <= ptr2.get();
}

template<class E1, class E2>
bool operator>=(const hol::access_ptr<E1>& ptr1, const hol::access_ptr<E2>& ptr2)
{
	return ptr1.get() >= ptr2.get();
}

namespace hol {

struct stdio_file_closer
{
	void operator()(FILE* fp) const { std::fclose(fp); }
};

#ifdef __unix__

struct piped_file_closer
{
	void operator()(FILE* fp) const { pclose(fp); }
};

using unique_piped_file_ptr = std::unique_ptr<FILE, piped_file_closer>;

using unique_PIPE_handle = unique_piped_file_ptr;

#endif

using unique_stdio_file_ptr = std::unique_ptr<FILE, stdio_file_closer>;

class shared_stdio_file_ptr
: private std::shared_ptr<FILE>
{
	using ptr = std::shared_ptr<FILE>;

public:
	shared_stdio_file_ptr(FILE* fp): ptr(fp, stdio_file_closer()) {}

	using ptr::reset;
	using ptr::swap;
	using ptr::operator=;

	using ptr::get;
	using ptr::operator*;
	using ptr::operator->;
	using ptr::unique;
	using ptr::operator bool;
	using ptr::owner_before;
};

inline
auto open_shared_stdio_file(std::string const& filename, std::string const& mode)
{
	return std::shared_ptr<FILE>(std::fopen(filename.c_str(), mode.c_str()), stdio_file_closer());
}

using unique_FILE_handle = std::unique_ptr<FILE, stdio_file_closer>;
using shared_FILE_handle = shared_stdio_file_ptr;
using access_FILE_handle = access_ptr<FILE>;

template<typename Handle, typename Cleaner = std::default_delete<Handle>>
class unique_handle
{
	Handle h;
	bool owner = false;

	void dispose();

public:
	unique_handle();
	unique_handle(Handle h);
	unique_handle(unique_handle&& handle);
	unique_handle(unique_handle const&) = delete;
	unique_handle& operator=(unique_handle&& handle);
	unique_handle& operator=(unique_handle const&) = delete;
	~unique_handle();

	bool operator==(unique_handle const& handle) const { return handle.h == h; }

	Handle& get();
	Handle const& get() const;
	Handle release();
	void reset();
	void reset(Handle h);
};

template<typename Handle, typename Cleaner>
void unique_handle<Handle, Cleaner>::dispose() { if(owner) Cleaner()(h); owner = false; }

template<typename Handle, typename Cleaner>
unique_handle<Handle, Cleaner>::unique_handle(): h(), owner(false) {}

template<typename Handle, typename Cleaner>
unique_handle<Handle, Cleaner>::unique_handle(Handle h): h(h), owner(true) {}

template<typename Handle, typename Cleaner>
unique_handle<Handle, Cleaner>::unique_handle(unique_handle&& handle)
{
	this->h = handle.h;
	this->owner = handle.owner;
	handle.owner = false;
}

template<typename Handle, typename Cleaner>
unique_handle<Handle, Cleaner>&
unique_handle<Handle, Cleaner>::operator=(
	unique_handle<Handle, Cleaner>&& handle)
{
	dispose();

	this->h = handle.h;
	this->owner = handle.owner;
	handle.owner = false;
}

template<typename Handle, typename Cleaner>
unique_handle<Handle, Cleaner>::~unique_handle() { dispose(); }

template<typename Handle, typename Cleaner>
void unique_handle<Handle, Cleaner>::reset() { dispose(); }

template<typename Handle, typename Cleaner>
void unique_handle<Handle, Cleaner>::reset(Handle h)
{
	dispose();
	this->h = h;
	this->owner = true;
}

template<typename Handle, typename Cleaner>
Handle& unique_handle<Handle, Cleaner>::get() { return h; }

template<typename Handle, typename Cleaner>
Handle const& unique_handle<Handle, Cleaner>::get() const { return h; }

template<typename Handle, typename Cleaner>
Handle unique_handle<Handle, Cleaner>::release() { owner = false; return h; }

} // hol

#endif // HOL_MEMORY_H
