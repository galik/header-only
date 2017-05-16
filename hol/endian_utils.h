#ifndef HEADER_ONLY_LIBRARY_ENDIAN_UTILS_H
#define HEADER_ONLY_LIBRARY_ENDIAN_UTILS_H
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

#include <cinttypes>
#include <algorithm>

#ifdef __cpp_if_constexpr
#define IF_CONSTEXPR(cond) if constexpr(cond)
#else
#define IF_CONSTEXPR(cond) if(cond)
#endif

namespace header_only_library {
namespace endian_utils {

constexpr bool is_littleendian()
{
	// C++11 Standard §9.5.5 should guarantee that this alias works
	// when coupled with the rule §3.10.10 that char can alias any other type
	// because it states that i and c[0] share the same address
	static union { unsigned i = 1; char c[1]; };
	return c[0];
}

constexpr bool is_bigendian()
{
	return !is_littleendian();
}

template<typename Numeric>
Numeric swap_endianness(Numeric n)
{
	std::reverse((char*)&n, ((char*)&n) + sizeof(n));
	return n;
}

template<typename Numeric>
constexpr Numeric from_littleendian(Numeric n)
{
	IF_CONSTEXPR(is_littleendian())
		return n;
	return swap_endianness(n);
}

template<typename Numeric>
constexpr Numeric from_bigendian(Numeric n)
{
	IF_CONSTEXPR(is_bigendian())
		return n;
	return swap_endianness(n);
}

} // namespace endian_utils
} // namespace header_only_library

#endif // HEADER_ONLY_LIBRARY_ENDIAN_UTILS_H
