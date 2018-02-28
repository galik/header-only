#ifndef HEADER_ONLY_LIBRARY_MISC_UTILS_H
#define HEADER_ONLY_LIBRARY_MISC_UTILS_H
//
// Copyright (c) 2017 Galik <galik.bool@gmail.com>
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

#include <cassert>
#include <iostream>

#include "assertions.h"

// VERSIONS

#if __cplusplus >= 199711L
#  define HOL_AT_LEAST_CPP98
#  define HOL_AT_LEAST_CPP03
#endif

#if __cplusplus >= 201103L
#  define HOL_AT_LEAST_CPP11
#endif

#if __cplusplus >= 201402L
#  define HOL_AT_LEAST_CPP14
#endif

#if __cplusplus >= 201703L
#  define HOL_AT_LEAST_CPP17
#endif

#ifdef __GNUG__
//
// Branch Prediction Hints
//
// Usage:
//
// if(HOL_LIKELY(my_condition(i)))
// {
//     // most visited code here
// }
//
#define HOL_LIKELY(x)    __builtin_expect(!!(x), 1)
#define HOL_UNLIKELY(x)  __builtin_expect(!!(x), 0)
#else
#define HOL_LIKELY(x)    (x)
#define HOL_UNLIKELY(x)  (x)
#endif

// Attributes

#ifndef __has_cpp_attribute
#define __has_cpp_attribute(name) 0
#endif

#ifndef HOL_NODISCARD
#if __has_cpp_attribute(nodiscard)
#define HOL_NODISCARD [[nodiscard]]
#else
#define HOL_NODISCARD
#endif
#endif // HOL_NODISCARD

#ifndef HOL_NORETURN
#if __has_cpp_attribute(noreturn)
#define HOL_NORETURN [[noreturn]]
#else
#define HOL_NORETURN
#endif
#endif // HOL_NORETURN

#ifndef HOL_DEPRECATED
#if __has_cpp_attribute(deprecated)
#define HOL_DEPRECATED(reason) [[deprecated(reason)]]
#else
#define HOL_DEPRECATED(reason)
#endif
#endif // HOL_DEPRECATED

#ifndef HOL_FALLTHROUGH
#if __has_cpp_attribute(fallthrough)
#define HOL_FALLTHROUGH [[fallthrough]]
#else
#define HOL_FALLTHROUGH
#endif
#endif // HOL_FALLTHROUGH

#ifndef HOL_MAYBE_UNUSED
#if __has_cpp_attribute(maybe_unused)
#define HOL_MAYBE_UNUSED [[maybe_unused]]
#else
#define HOL_MAYBE_UNUSED
#endif
#endif // HOL_MAYBE_UNUSED

// casting

namespace header_only_library {
namespace misc_utils {

template <class T, class U>
inline T narrow_cast(U u) noexcept
{
	T t = static_cast<T>(u);
	HOL_ASSERT_MSG(static_cast<U>(t) == u, "narrowing changed the value from: " << std::size_t(u) << " to: " << std::size_t(t));
	HOL_ASSERT_MSG_IF(std::is_signed<T>::value != std::is_signed<U>::value,(t < T{}) == (u < U{}),
		"narrowing changed the sign of the value from: " << std::size_t(u) << " to: " << std::size_t(t));
	return t;
}

template<typename Func>
class HOL_NODISCARD scoped_action
{
public:
	explicit scoped_action(Func func): func(std::move(func)), active(true) {}
	~scoped_action() { if(active) func(); }

	scoped_action(scoped_action const&) = delete;
	scoped_action& operator=(scoped_action const&) = delete;

	scoped_action(scoped_action&& other): func(std::move(other.func))
		{ other.active = false; }

private:
	Func func;
	bool active = true;
};

template<typename Func>
scoped_action<Func> finally(Func&& func)
{
	return scoped_action<Func>(std::forward<Func>(func));
}

} // namespace misc_utils
} // namespace header_only_library

#endif // HEADER_ONLY_LIBRARY_MISC_UTILS_H
