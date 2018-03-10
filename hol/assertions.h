#ifndef HEADER_ONLY_LIBRARY_ASSERTIONS_H
#define HEADER_ONLY_LIBRARY_ASSERTIONS_H
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

#include <iostream>
#include <sstream>
#include <stdexcept>

#ifndef HOL_ASSERT_THROWS
#  define HOL_ASSERT_ACTION(msg) do { \
	std::cerr << msg << '\n'; \
	std::abort(); \
	} while(0)
#else
#  define HOL_ASSERT_ACTION(msg) \
	do { \
	std::ostringstream o; \
	o << msg; \
	throw std::runtime_error(o.str()); \
	}while(0)
#endif

#ifdef NDEBUG
#define HOL_ASSERT(expr) do{}while(0)
#define HOL_ASSERT_IF(cond,expr) do{}while(0)
#define HOL_ASSERT_MSG(expr,msg) do{}while(0)
#define HOL_ASSERT_MSG_IF(cond,expr,msg) do{}while(0)
#else
#define HOL_ASSERT(expr) \
do { \
	if(!(expr)) \
	{ \
		HOL_ASSERT_ACTION("assertion '" << #expr << "' failed." << "\nfile: " << __FILE__ << "\nline: " << __LINE__ << '\n'); \
	} \
}while(0)
#define HOL_ASSERT_IF(cond,expr) do { if(cond) HOL_ASSERT(expr); }while(0)
#define HOL_ASSERT_MSG(expr,msg) \
do { \
	if(!(expr)) \
	{ \
		HOL_ASSERT_ACTION("assertion '" << #expr << "' failed.\ninfo: " << msg << "\nfile: " << __FILE__ << "\nline: " << __LINE__ << '\n'); \
	} \
}while(0)
#define HOL_ASSERT_MSG_IF(cond,expr,msg) do { if(cond) HOL_ASSERT_MSG(expr,msg); }while(0)
#endif

#endif // HEADER_ONLY_LIBRARY_ASSERTIONS_H
