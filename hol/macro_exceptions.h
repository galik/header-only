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
#include <string>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <functional>

#ifdef NDEBUG

#define HOL_THROW_EXCEPTION(exc, message) \
	throw exc(message)

#else

#define HOL_THROW_EXCEPTION(exc, msg) do{ \
	std::ostringstream oss; \
	oss << __FILE__ << ":" << __LINE__ << ":error: " << msg; \
	throw exc(oss.str());}while(0)

#endif // NDEBUG

#define HOL_THROW_RUNTIME_ERROR(msg) \
	HOL_THROW_EXCEPTION(std::runtime_error, msg)

#define HOL_THROW_ERRNO() HOL_THROW_RUNTIME_ERROR(std::strerror(errno))

#endif // HOL_MACRO_EXCEPTIONS_H
