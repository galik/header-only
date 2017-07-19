#ifndef HOL_MACRO_EXCEPTIONS_H
#define HOL_MACRO_EXCEPTIONS_H
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

#include <cerrno>
#include <sstream>
#include <stdexcept>

#ifdef NDEBUG

#define hol_throw_exception(e, m) do{std::ostringstream o;o<<m;throw e(o.str());}while(0)

#else

#define hol_throw_exception(e, m) do{ \
	std::ostringstream o; \
	o << __FILE__ << ":" << __LINE__ << ":error: " << m; \
	throw e(o.str());}while(0)

#endif // NDEBUG

#define hol_throw_runtime_error(m) hol_throw_exception(std::runtime_error, m)
#define hol_throw_errno() hol_throw_runtime_error(std::strerror(errno))
#define hol_throw_errno_msg(m) hol_throw_runtime_error(m << ": " << std::strerror(errno))

// Moved to macro_utils.h
//#ifdef __GNUG__
////
//// Branch Prediction Hints
////
//// Usage:
////
//// if(likely(my_condition(i)))
//// {
////     // most visited code here
//// }
////
//#define hol_likely(x)    __builtin_expect(!!(x), 1)
//#define hol_unlikely(x)  __builtin_expect(!!(x), 0)
//#else
//#define hol_likely(x)    (x)
//#define hol_unlikely(x)  (x)
//#endif

#endif // HOL_MACRO_EXCEPTIONS_H
