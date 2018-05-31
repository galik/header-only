#ifndef HEADER_ONLY_LIBRARY_TYPE_UTILS_H
#define HEADER_ONLY_LIBRARY_TYPE_UTILS_H
//
// Copyright (c) 2018 Galik <galik.bool@gmail.com>
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

#include <cstddef>

namespace header_only_library {
namespace type_utils {

namespace whole_types {

using signed_char = signed char;
using signed_short = signed short;
using signed_int = signed int;
using signed_long = signed long;
using signed_long_long = signed long long;

using unsigned_char = unsigned char;
using unsigned_short = unsigned short;
using unsigned_int = unsigned int;
using unsigned_long = unsigned long;
using unsigned_long_long = unsigned long long;

using long_double = long double;

using signed_char_ptr = signed char*;
using signed_char_ref = signed char&;
using signed_short_ptr = signed short*;
using signed_short_ref = signed short&;
using signed_int_ptr = signed int*;
using signed_int_ref = signed int&;
using signed_long_ptr = signed long*;
using signed_long_ref = signed long&;
using signed_long_long_ptr = signed long long*;
using signed_long_long_ref = signed long long&;

using unsigned_char_ptr = unsigned char*;
using unsigned_char_ref = unsigned char&;
using unsigned_short_ptr = unsigned short*;
using unsigned_short_ref = unsigned short&;
using unsigned_int_ptr = unsigned int*;
using unsigned_int_ref = unsigned int&;
using unsigned_long_ptr = unsigned long*;
using unsigned_long_ref = unsigned long&;
using unsigned_long_long_ptr = unsigned long long*;
using unsigned_long_long_ref = unsigned long long&;

using float_ptr = float*;
using float_ref = float&;

using long_double_ptr = long double*;
using long_double_ref = long double&;

using signed_char_const_ptr = signed char const*;
using signed_char_const_ref = signed char const&;
using signed_short_const_ptr = signed short const*;
using signed_short_const_ref = signed short const&;
using signed_int_const_ptr = signed int const*;
using signed_int_const_ref = signed int const&;
using signed_long_const_ptr = signed long const*;
using signed_long_const_ref = signed long const&;
using signed_long_long_const_ptr = signed long long const*;
using signed_long_long_const_ref = signed long long const&;

using unsigned_char_const_ptr = unsigned char const*;
using unsigned_char_const_ref = unsigned char const&;
using unsigned_short_const_ptr = unsigned short const*;
using unsigned_short_const_ref = unsigned short const&;
using unsigned_int_const_ptr = unsigned int const*;
using unsigned_int_const_ref = unsigned int const&;
using unsigned_long_const_ptr = unsigned long const*;
using unsigned_long_const_ref = unsigned long const&;
using unsigned_long_long_const_ptr = unsigned long long const*;
using unsigned_long_long_const_ref = unsigned long long const&;

using float_const_ptr = float const*;
using float_const_ref = float const&;

using long_double_const_ptr = long double const*;
using long_double_const_ref = long double const&;

} // namespace whole_types

using index = std::ptrdiff_t;

} // namespace type_utils
} // namespace header_only_library

#endif /* HEADER_ONLY_LIBRARY_TYPE_UTILS_H_ */
