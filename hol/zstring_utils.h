#ifndef HEADER_ONLY_LIBRARY_ZSTRING_UTILS_H
#define HEADER_ONLY_LIBRARY_ZSTRING_UTILS_H
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

#include <cstring>

namespace header_only_library {
namespace zstring_utils {

template<class T>
struct make_unsigned; // no implementation

template<>
struct make_unsigned<char> { using type = unsigned char; };

template<>
struct make_unsigned<unsigned char> { using type = unsigned char; };

template<>
struct make_unsigned<wchar_t> { using type = wchar_t; };

template<>
struct make_unsigned<char16_t> { using type = char16_t; };

template<>
struct make_unsigned<char32_t> { using type = char32_t; };

// TODO: This is still MUCH slower that std::strcmp

template<typename CharT>
bool equal_to(CharT const* s1, CharT const* s2)
{
	while(*s1 && *s1 == *s2)
		s1++, s2++;

	return *s1 == *s2;
}

template<typename CharT>
bool less_than(CharT const* s1, CharT const* s2)
{
	while(*s1 && *s1 == *s2)
		s1++, s2++;

	return unsigned(*s1) < unsigned(*s2);
}

template<typename CharT>
bool less_than_or_equal_to(CharT const* s1, CharT const* s2)
{
	while(*s1 && *s1 == *s2)
		s1++, s2++;

	return unsigned(*s1) <= unsigned(*s2);
}

template<typename CharT>
bool greater_than(CharT const* s1, CharT const* s2)
{
	while(*s1 && *s1 == *s2)
		s1++, s2++;

	return unsigned(*s1) > unsigned(*s2);
}

template<typename CharT>
bool greater_than_or_equal_to(CharT const* s1, CharT const* s2)
{
	while(*s1 && *s1 == *s2)
		s1++, s2++;

	return unsigned(*s1) >= unsigned(*s2);
}

template<typename CharT>
std::size_t length_of(CharT const* s)
{
	auto p = s;
	while(*p) ++p;
	return std::size_t(p - s);
}

template<typename CharT>
std::size_t length_of_n(CharT const* s, std::size_t n)
{
    return std::find(s, s + n, CharT(0)) - s;
}

} // namespace zstring_utils

namespace generic {

template<typename CharT>
CharT* strcpy(CharT* dst, char const* src)
{
	auto ptr = dst;
	while((*ptr++ = *src++)) {}
	return dst;
}

template<typename CharT>
std::size_t strlen(const CharT* s)
{
	auto p = s;
	while(*p) ++p;
	return std::size_t(p - s);
}

template<typename CharT>
auto strnlen(const CharT* s, std::size_t n)
{
	return std::find(s, s + n, CharT(0)) - s;
}


} // namespace generic


} // namespace header_only_library

#endif // HEADER_ONLY_LIBRARY_ZSTRING_UTILS_H
