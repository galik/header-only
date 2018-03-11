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

#include <cstddef>
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
//	return !std::strcmp((char const*) s1, (char const*) s1);
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

namespace generic {

//  String manipulation

inline
char* strcpy(char* dst, char const* src) { return std::strcpy(dst, src); }

template<typename CharT>
CharT* strcpy(CharT* dst, CharT const* src)
{
	auto ptr = dst;
	while((*ptr++ = *src++)) {}
	return dst;
}

inline
char* strncpy(char* dst, char const* src, std::size_t n) { return std::strncpy(dst, src, n); }

template<typename CharT>
CharT* strncpy(CharT* dst, CharT const* src, std::size_t n)
{
	auto ptr = dst;
	while(n && (*ptr++ = *src++)) --n;
	while(n) { *ptr++ = CharT(0); --n; }
	return dst;
}

inline
char* strcat(char* dst, char const* src) { return std::strcat(dst, src); }

template<typename CharT>
CharT* strcat(CharT* dst, CharT const* src)
{
	auto ptr = dst;
	while(*ptr) ++ptr;
	while((*ptr++ = *src++)) {}
	return dst;
}

inline
char* strncat(char* dst, char const* src, std::size_t n) { return std::strncat(dst, src, n); }

template<typename CharT>
CharT* strncat(CharT* dst, CharT const* src, std::size_t n)
{
	if(n)
	{
		auto ptr = dst;
		while(*ptr) ++ptr;
		while(n && (*ptr++ = *src++)) --n;
		*ptr = CharT(0);
	}
	return dst;
}

//  String examination

inline
std::size_t strlen(char const* s) { return std::strlen(s); }

template<typename CharT>
std::size_t strlen(CharT const* s)
{
	auto p = s;
	while(*p) ++p;
	return std::size_t(p - s);
}

inline
std::size_t strnlen(char const* s, std::size_t n) { return ::strnlen(s, n); }

template<typename CharT>
std::size_t strnlen(CharT const* s, std::size_t n)
{
	return std::find(s, s + n, CharT(0)) - s;
}

inline
int strcmp(char const* lhs, char const* rhs) { return std::strcmp(lhs, rhs); }

template<typename CharT>
int strcmp(CharT const* lhs, CharT const* rhs)
{
	using unsigned_char = typename make_unsigned<CharT>::type;

	while(*lhs && *lhs == *rhs) ++lhs, ++rhs;
	return *((unsigned_char const*)lhs) - *((unsigned_char const*)rhs);
}

inline
int strncmp(char const* lhs, char const* rhs, std::size_t n) { return std::strncmp(lhs, rhs, n); }

template<typename CharT>
int strncmp(CharT const* lhs, CharT const* rhs, std::size_t n)
{
	using unsigned_char = typename make_unsigned<CharT>::type;

	if(!n)
		return 0;

	while(--n && *lhs && *lhs == *rhs) ++lhs, ++rhs;
	return *((unsigned_char const*)lhs) - *((unsigned_char const*)rhs);
}

// TODO: Test this, original signature strchr(char const*, int)

inline
char* strchr(char* s, char c) { return std::strchr(s, c); }

inline
char const* strchr(char const* s, char c) { return std::strchr(s, c); }

template<typename CharT>
CharT* strchr(CharT* s, CharT c)
{
	while(*s && *s != CharT(c)) ++s;
	return *s == CharT(c) ? s : nullptr;
}

//template<typename CharT>
//CharT* strchr(CharT* s, CharT c)
//{
//	return const_cast<CharT*>(strchr(const_cast<CharT const*>(s), c));
//}

char* strrchr(char* s, char c) { return std::strrchr(s, c); }
char const* strrchr(char const* s, char c) { return std::strrchr(s, c); }

template<typename CharT>
CharT* strrchr(CharT* s, int c)
{
	auto p = s + strlen(s);
	while(p >= s && *p != CharT(c)) --p;
	return p >= s ? p : nullptr;
}

//template<typename CharT>
//CharT* strrchr(CharT* s, int c)
//{
//	return const_cast<CharT*>(strrchr(const_cast<CharT const*>(s), c));
//}

inline
std::size_t strspn(char const* s1, char const* s2) { return std::strspn(s1, s2); }

template<typename CharT>
std::size_t strspn(CharT const* s1, CharT const* s2)
{
	std::size_t n = 0;
	while(*s1 && strchr(s2, *s1)) ++s1, ++n;
	return n;
}

inline
std::size_t strcspn(char const* s1, char const* s2) { return std::strcspn(s1, s2); }

template<typename CharT>
std::size_t strcspn(CharT const* s1, CharT const* s2)
{
	std::size_t n = 0;
	while(*s1 && !strchr(s2, *s1)) ++s1, ++n;
	return n;
}

inline
char* strpbrk(char* s, char const* brk) { return std::strpbrk(s, brk); }

inline
char const* strpbrk(char const* s, char const* brk) { return std::strpbrk(s, brk); }

// TODO: Optimize (don't use strlen)
template<typename CharT>
CharT* strpbrk(CharT* s, CharT const* brk)
{
	auto f = std::find_first_of(s, s + strlen(s), brk, brk + strlen(brk));

	if(f != s + strlen(s))
		return f;

	return {};
}

//template<typename CharT>
//CharT* strpbrk(CharT* s, CharT const* brk)
//{
//	return const_cast<CharT*>(strpbrk(const_cast<CharT const*>(s), brk));
//}

char* strstr(char* s1, char const* s2) { return std::strstr(s1, s2); }
char const* strstr(char const* s1, char const* s2) { return std::strstr(s1, s2); }

// TODO: Optimize (don't use strlen)
template<typename CharT>
CharT* strstr(CharT* s1, CharT const* s2)
{
	auto f = std::search(s1, s1 + strlen(s1), s2, s2 + strlen(s2));

	if(f != s1 + strlen(s1))
		return f;

	return {};
}

//template<typename CharT>
//CharT* strstr(CharT* s1, CharT const* s2)
//{
//	return const_cast<CharT*>(strstr(const_cast<CharT const*>(s1), s2));
//}

char* strtok(char* s, char const* delim) { return std::strtok(s, delim); }

template<typename CharT>
CharT* strtok(CharT* s, CharT const* delim)
{
	thread_local static CharT* p = {};

	if(!s)
		s = p;

	if(s)
	{
		p = nullptr;

		auto tok = s + strcspn(s, delim);

		if(!*(tok))
			return nullptr;

		s = tok + strspn(tok, delim);

		if(*s)
		{
			*s = {};
			p = s + 1;
		}

		return tok;
	}

	return p;
}

} // namespace generic
} // namespace zstring_utils
} // namespace header_only_library

#endif // HEADER_ONLY_LIBRARY_ZSTRING_UTILS_H
