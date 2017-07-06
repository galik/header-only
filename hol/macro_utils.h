#ifndef HOL_MACRO_UTILS_H
#define HOL_MACRO_UTILS_H
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

// Version Utils

// AT LEAST
#if __cplusplus >= 199711L
#define HOL_IF_AT_LEAST_CPP98(code) do{code}while(0)
#define HOL_IF_AT_LEAST_CPP03(code) do{code}while(0)
#else
#define HOL_IF_AT_LEAST_CPP98(code) do{}while(0)
#define HOL_IF_AT_LEAST_CPP03(code) do{}while(0)
#endif

#if __cplusplus >= 201103L
#define HOL_IF_AT_LEAST_CPP11(code) do{code}while(0)
#else
#define HOL_IF_AT_LEAST_CPP11(code) do{}while(0)
#endif

#if __cplusplus >= 201402L
#define HOL_IF_AT_LEAST_CPP14(code) do{code}while(0)
#else
#define HOL_IF_AT_LEAST_CPP14(code) do{}while(0)
#endif

#if __cplusplus >= 201703L
#define HOL_IF_AT_LEAST_CPP17(code) do{code}while(0)
#else
#define HOL_IF_AT_LEAST_CPP17(code) do{}while(0)
#endif

#ifdef NDEBUG
#define HOL_IF_DEBUG(code) do{}while(0)
#else
#define HOL_IF_DEBUG(code) do{code}while(0)
#endif

// BEFORE

#if __cplusplus < 199711L
#define HOL_IF_BEFORE_CPP98(code) do{code}while(0)
#define HOL_IF_BEFORE_CPP03(code) do{code}while(0)
#else
#define HOL_IF_BEFORE_CPP98(code) do{}while(0)
#define HOL_IF_BEFORE_CPP03(code) do{}while(0)
#endif

#if __cplusplus < 201103L
#define HOL_IF_BEFORE_CPP11(code) do{code}while(0)
#else
#define HOL_IF_BEFORE_CPP11(code) do{}while(0)
#endif

#if __cplusplus < 201402L
#define HOL_IF_BEFORE_CPP14(code) do{code}while(0)
#else
#define HOL_IF_BEFORE_CPP14(code) do{}while(0)
#endif

#if __cplusplus < 201703L
#define HOL_IF_BEFORE_CPP17(code) do{code}while(0)
#else
#define HOL_IF_BEFORE_CPP17(code) do{}while(0)
#endif

// EXACTLY

#if __cplusplus == 199711L
#define HOL_IF_EXACTLY_CPP98(code) do{code}while(0)
#define HOL_IF_EXACTLY_CPP03(code) do{code}while(0)
#else
#define HOL_IF_EXACTLY_CPP98(code) do{}while(0)
#define HOL_IF_EXACTLY_CPP03(code) do{}while(0)
#endif

#if __cplusplus == 201103L
#define HOL_IF_EXACTLY_CPP11(code) do{code}while(0)
#else
#define HOL_IF_EXACTLY_CPP11(code) do{}while(0)
#endif

#if __cplusplus == 201402L
#define HOL_IF_EXACTLY_CPP14(code) do{code}while(0)
#else
#define HOL_IF_EXACTLY_CPP14(code) do{}while(0)
#endif

#if __cplusplus == 201703L
#define HOL_IF_EXACTLY_CPP17(code) do{code}while(0)
#else
#define HOL_IF_EXACTLY_CPP17(code) do{}while(0)
#endif

// ASERTS

#ifdef NDEBUG
#define HOL_ASSERT(expr) do{}while(0)
#define HOL_ASSERT_MSG(expr, msg) do{}while(0)
#else
#define HOL_ASSERT(expr) \
do { \
	if(!(expr)) \
	{ \
		std::cerr << "assertion '" << #expr << "' failed." << "\nfile: " << __FILE__ << "\nline: " << __LINE__ << '\n'; \
		std::abort(); \
	} \
}while(0)
#define HOL_ASSERT_MSG(expr, msg) \
do { \
	if(!(expr)) \
	{ \
		std::cerr << "assertion '" << #expr << "' failed.\ninfo: " << msg << "\nfile: " << __FILE__ << "\nline: " << __LINE__ << '\n'; \
		std::abort(); \
	} \
}while(0)
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

#endif // HOL_MACRO_UTILS_H
