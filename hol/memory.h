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

namespace hol {

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

#endif // HOL_MEMORY_H
