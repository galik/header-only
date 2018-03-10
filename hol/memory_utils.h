#ifndef HEADER_ONLY_LIBRARY_MEMORY_UTILS_H
#define HEADER_ONLY_LIBRARY_MEMORY_UTILS_H
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

namespace header_only_library {
namespace memory_utils {

struct malloc_dter{void operator()(void*p)const{std::free(p);}};

template<typename T>
using malloc_uptr = std::unique_ptr<T, malloc_dter>;

using calloc_uptr = malloc_uptr<char>;


struct stdio_file_closer
	{ void operator()(std::FILE* fp) const { if(fp) std::fclose(fp); } };

#ifdef __unix__

struct piped_file_closer
	{ void operator()(std::FILE* fp) const { if(fp) pclose(fp); } };

using unique_piped_file_ptr = std::unique_ptr<std::FILE, piped_file_closer>;

using unique_PIPE_handle = unique_piped_file_ptr;

#endif

using unique_stdio_file_ptr = std::unique_ptr<std::FILE, stdio_file_closer>;

class shared_stdio_file_ptr
: private std::shared_ptr<std::FILE>
{
	using ptr = std::shared_ptr<std::FILE>;

public:
	shared_stdio_file_ptr(std::FILE* fp): ptr(fp, stdio_file_closer()) {}

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
std::shared_ptr<std::FILE> open_shared_stdio_file(std::string const& filename, std::string const& mode)
{
	return std::shared_ptr<std::FILE>(std::fopen(filename.c_str(), mode.c_str()), stdio_file_closer());
}

using unique_FILE_handle = std::unique_ptr<std::FILE, stdio_file_closer>;
using shared_FILE_handle = shared_stdio_file_ptr;
//using access_FILE_handle = access_ptr<std::FILE>;

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

	operator bool() const { return owner; }
};

template<typename Handle, typename Cleaner>
void unique_handle<Handle, Cleaner>::dispose() { if(owner) Cleaner()(h); owner = false; }

template<typename Handle, typename Cleaner>
unique_handle<Handle, Cleaner>::unique_handle(): h(), owner(false) {}

template<typename Handle, typename Cleaner>
unique_handle<Handle, Cleaner>::unique_handle(Handle h): h(h), owner(true) {}

template<typename Handle, typename Cleaner>
unique_handle<Handle, Cleaner>::unique_handle(unique_handle&& handle)
: h(std::move(handle.h)), owner(handle.owner)
{
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
	return *this;
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

} // namespace memory_utils
} // namespace header_only_library

#endif // HEADER_ONLY_LIBRARY_MEMORY_UTILS_H
