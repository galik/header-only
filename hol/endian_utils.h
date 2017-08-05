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

#ifndef IF_CONSTEXPR
#ifdef __cpp_if_constexpr
#define IF_CONSTEXPR(cond) if constexpr(cond)
#else
#define IF_CONSTEXPR(cond) if(cond)
#endif // __cpp_if_constexpr
#endif // IF_CONSTEXPR

namespace header_only_library {
namespace endian_utils {

enum class endian
{
	unknown = 0,
	little = __ORDER_LITTLE_ENDIAN__,
	big    = __ORDER_BIG_ENDIAN__,
	native = __BYTE_ORDER__
};

constexpr bool is_little_endian()
{
	return endian::native == endian::little;
}

constexpr bool is_big_endian()
{
	return !is_little_endian();
}

template<typename Numeric>
Numeric swap_endianness(Numeric n)
{
	std::reverse((char*)&n, ((char*)&n) + sizeof(n));
	return n;
}

template<typename Numeric>
constexpr Numeric from_little_endian(Numeric n)
{
	IF_CONSTEXPR(is_little_endian())
		return n;
	return swap_endianness(n);
}

template<typename Numeric>
constexpr Numeric from_big_endian(Numeric n)
{
	IF_CONSTEXPR(is_big_endian())
		return n;
	return swap_endianness(n);
}

template<typename Numeric>
endian discover_endianness(Numeric const& field, Numeric const&& expected_value)
{
	if(from_little_endian(field) == expected_value)
		return endian::little;
	else if(from_big_endian(field))
		return endian::big;
	return endian::unknown;
}

template<typename Numeric>
constexpr Numeric from_endian(endian e, Numeric n)
{
	if(e == endian::little)
		return from_little_endian(n);
	else if(e == endian::big)
		return from_big_endian(n);
	return n;
}

// I'll just put these here for now (they may move)

template<typename Char, typename T>
Char* encode_network_byte_order(T const& t, Char* data)
{
	static_assert(std::is_same<Char*, char*>::value
		|| std::is_same<Char*, unsigned char*>::value
#if __cplusplus >= 201703
		|| std::is_same<Char*, std::byte*>::value
#endif
		,	"Second parmeter must be a char*, unsigned char* or std::byte*");

//	using char_type = typename std::remove_const<typename std::remove_pointer<CharPtr>::type>::type;

	IF_CONSTEXPR(is_big_endian())
		std::copy((Char const*)&t, (Char const*)&t + sizeof(T), data);
	else
		std::reverse_copy((Char const*)&t, (Char const*)&t + sizeof(T), data);

	return data + sizeof(T);
}

template<typename Char, typename T>
Char const* decode_network_byte_order(Char const* data, T& t)
{
//	bug_fun();
//	using char_type = typename std::remove_const<typename std::remove_pointer<CharPtr>::type>::type;

	static_assert(std::is_same<Char*, char*>::value
		|| std::is_same<Char*, unsigned char*>::value
#if __cplusplus >= 201703
		|| std::is_same<Char*, std::byte*>::value
#endif
		, "Second parmeter must be a char const*, unsigned char const* or std::byte const*");

	IF_CONSTEXPR(is_big_endian())
		std::copy(data, data + sizeof(T), (Char*)&t);
	else
		std::reverse_copy(data, data + sizeof(T), (Char*)&t);

	return data + sizeof(T);
}

} // namespace endian_utils
} // namespace header_only_library

#endif // HEADER_ONLY_LIBRARY_ENDIAN_UTILS_H
