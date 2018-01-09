#ifndef HOL_UNICODE_UTILS_H
#define HOL_UNICODE_UTILS_H
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


// TODO: Look at this: http://bjoern.hoehrmann.de/utf-8/decoder/dfa/

#include <codecvt>
#include <locale>
#include <stdexcept>
#include <string>

namespace header_only_library {
namespace unicode_utils {
namespace detail {

//using wcodecvt_utf8 = std::codecvt_utf8<wchar_t>;
//using ucs2codecvt_utf8 = std::codecvt_utf8<char16_t>;
//using ucs4codecvt_utf8 = std::codecvt_utf8<char32_t>;
//using utf16codecvt_utf8 = std::codecvt_utf8_utf16<char16_t>;
//using utf32codecvt_utf8 = std::codecvt_utf8<char32_t>;
//using utf32codecvt_utf16 = std::codecvt_utf16<char32_t>; // ??

using cvt_ws_utf8 = std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>;
using cvt_ucs2_utf8 = std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t>;
using cvt_ucs4_utf8 = std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>;
using cvt_utf16_utf8 = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>;
using cvt_utf32_utf8 = std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>;
using cvt_utf32_utf16 = std::wstring_convert<std::codecvt_utf16<char32_t>, char32_t>; // ??

template<typename CodeCvt, typename FromString, typename ToString = std::string>
ToString convert_to(FromString const& s)
{
	CodeCvt cnv;
	ToString to = cnv.to_bytes(s);
	if(cnv.converted() < s.size())
		throw std::runtime_error("incomplete conversion");
	return to;
}

template<typename CodeCvt, typename ToString, typename FromString = std::string>
ToString convert_from(FromString const& s)
{
	CodeCvt cnv;
	ToString to = cnv.from_bytes(s);
	if(cnv.converted() < s.size())
		throw std::runtime_error("incomplete conversion");
	return to;
}

} //namespace detail

// UTF-32/UTF-16/UCS-4/UCS-2 to UTF-8/UTF-16

inline
std::string ucs2_to_utf8(std::u16string const& ucs2)
{
	return detail::convert_to<detail::cvt_ucs2_utf8, std::u16string>(ucs2);
}

inline
std::string utf16_to_utf8(std::u16string const& utf16)
{
	return detail::convert_to<detail::cvt_utf16_utf8, std::u16string>(utf16);
}

inline
std::string ucs4_to_utf8(std::u32string const& ucs4)
{
	return detail::convert_to<detail::cvt_ucs4_utf8, std::u32string>(ucs4);
}

inline
std::string utf32_to_utf8(std::u32string const& utf32)
{
	return detail::convert_to<detail::cvt_utf32_utf8, std::u32string>(utf32);
}

inline
std::string ws_to_utf8(std::wstring const& s)
{
	return detail::convert_to<detail::cvt_ws_utf8, std::wstring>(s);
}

//inline
//std::u16string utf32_to_utf16(std::u32string const& s)
//{
//	return detail::convert_to<detail::cvt_utf32_utf16, std::u32string, std::u16string>(s);
//}

// UTF-8/UTF-16 to UTF-32/UTF-16/UCS-4/UCS-2

inline
std::u16string utf8_to_ucs2(std::string const& utf8)
{
	return detail::convert_from<detail::cvt_ucs2_utf8, std::u16string>(utf8);
}

inline
std::u16string utf8_to_utf16(std::string const& utf8)
{
	return detail::convert_from<detail::cvt_utf16_utf8, std::u16string>(utf8);
}

inline
std::u32string utf8_to_ucs4(std::string const& utf8)
{
	return detail::convert_from<detail::cvt_ucs4_utf8, std::u32string>(utf8);
}

inline
std::u32string utf8_to_utf32(std::string const& utf8)
{
	return detail::convert_from<detail::cvt_utf32_utf8, std::u32string>(utf8);
}

inline
std::wstring utf8_to_ws(std::string const& utf8)
{
	return detail::convert_from<detail::cvt_ws_utf8, std::wstring>(utf8);
}

//inline
//std::u32string utf16_to_utf32(std::u16string const& utf16)
//{
//	return detail::convert_from<detail::cvt_utf32_utf16, std::u32string, std::u16string>(utf16);
//}

// locale sensitive conversion between multibyte and wide characters

inline
std::wstring mb_to_ws(std::string const& mb)
{
	std::wstring ws;
	std::mbstate_t ps{};
	char const* src = mb.data();

	std::size_t len = 1 + std::mbsrtowcs(0, &src, 3, &ps);

	ws.resize(len);
	src = mb.data();

	std::mbsrtowcs(&ws[0], &src, ws.size(), &ps);

	if(src)
		throw std::runtime_error("invalid multibyte character after: '"
			+ std::string(mb.data(), src) + "'");

	ws.pop_back();

	return ws;
}

inline
std::string ws_to_mb(std::wstring const& ws)
{
	std::string mb;
	std::mbstate_t ps{};
	wchar_t const* src = ws.data();

	std::size_t len = 1 + std::wcsrtombs(0, &src, 0, &ps);

	mb.resize(len);
	src = ws.data();

	std::wcsrtombs(&mb[0], &src, mb.size(), &ps);

	if(src)
		throw std::runtime_error("invalid wide character");

	mb.pop_back();

	return mb;
}

// Codepoint conversions

inline
std::string codepoint_to_utf8(char32_t cp)
{
    char utf8[4];
    char* end_of_utf8;

	char32_t const* from = &cp;

	std::mbstate_t mbs;
	std::codecvt_utf8<char32_t> ccv;

	if(ccv.out(mbs, from, from + 1, from, utf8, utf8 + 4, end_of_utf8))
		throw std::runtime_error("bad conversion");

	return {utf8, end_of_utf8};
}

// UTF-16 may contain either one or two char16_t characters so
// we return a string to potentially contain both.
//
inline
std::u16string codepoint_to_utf16(char32_t cp)
{
	// convert UTF-32 (standard unicode codepoint) to UTF-8 intermediate value
    char utf8[4];
    char* end_of_utf8;

    {
        char32_t const* from = &cp;

        std::mbstate_t mbs;
	    std::codecvt_utf8<char32_t> ccv;

		if(ccv.out(mbs, from, from + 1, from, utf8, utf8 + 4, end_of_utf8))
			throw std::runtime_error("bad conversion");
    }

    // Now convert the UTF-8 intermediate value to UTF-16

    char16_t utf16[2];
    char16_t* end_of_utf16;

    {
        char const* from = nullptr;

		std::mbstate_t mbs;
	    std::codecvt_utf8_utf16<char16_t> ccv;

		if(ccv.in(mbs, utf8, end_of_utf8, from, utf16, utf16 + 2, end_of_utf16))
			throw std::runtime_error("bad conversion");
    }

    return {utf16, end_of_utf16};
}

} // unicode_utils
} // header_only_library

#endif // HOL_UNICODE_UTILS_H
