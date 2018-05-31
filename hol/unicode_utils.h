#ifndef HEADER_ONLY_LIBRARY_UNICODE_UTILS_H
#define HEADER_ONLY_LIBRARY_UNICODE_UTILS_H
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

#include <cassert>
#include <codecvt>
#include <locale>
#include <stdexcept>
#include <string>

// std::byte
#if __cplusplus >= 201703L
#  include <cstddef>
#endif

namespace header_only_library {
namespace unicode_utils {
namespace detail {

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

/**
 * This string class uses `UTF-32` internally but accepts and
 * outputs `UTF-8`.
 */
class utf32_string
{
public:
	utf32_string() {}
	utf32_string(char const* utf8): m_data(utf8_to_utf32(std::string(utf8))) {}
	utf32_string(std::string const& utf8): m_data(utf8_to_utf32(utf8)) {}
	utf32_string(std::u32string const& utf32): m_data(utf32) {}

	utf32_string& operator=(std::string const& utf8)
		{ m_data = utf8_to_utf32(utf8); return *this; }

	utf32_string& operator=(char const* utf8)
		{ return *this = std::string(utf8); }

	std::size_t size() const { return m_data.size(); }

	char32_t& operator[](std::size_t n)
		{ assert(n <= m_data.size()); return m_data[n]; }

	char32_t const& operator[](std::size_t n) const
		{ assert(n <= m_data.size()); return m_data[n]; }

	friend utf32_string operator+(utf32_string const& s1, char32_t c)
		{ return utf32_string(s1.m_data + c); }

	friend utf32_string operator+(utf32_string const& s1, utf32_string const& s2)
		{ return utf32_string(s1.m_data + s2.m_data); }

	friend utf32_string operator+(utf32_string const& utf32, char const* utf8)
		{ return utf32 + utf32_string(utf8); }

	friend utf32_string operator+(char const* utf8, utf32_string const& utf32)
		{ return utf32_string(utf8) + utf32; }

	friend utf32_string operator+(utf32_string const& utf32, std::string const& utf8)
		{ return utf32 + utf32_string(utf8); }

	friend utf32_string operator+(std::string const& utf8, utf32_string const& utf32)
		{ return utf32_string(utf8) + utf32; }


	friend bool operator==(utf32_string const& s1, utf32_string const& s2)
		{ return s1.m_data == s2.m_data; }

	friend bool operator==(utf32_string const& utf32, char const* utf8)
		{ return utf32 == utf32_string(utf8); }

	friend bool operator==(char const* utf8, utf32_string const& utf32)
		{ return utf32_string(utf8) == utf32; }

	friend bool operator==(utf32_string const& utf32, std::string const& utf8)
		{ return utf32 == utf32_string(utf8); }

	friend bool operator==(std::string const& utf8, utf32_string const& utf32)
		{ return utf32_string(utf8) == utf32; }


	friend bool operator!=(utf32_string const& s1, utf32_string const& s2)
		{ return s1.m_data != s2.m_data; }

	friend bool operator!=(utf32_string const& utf32, char const* utf8)
		{ return utf32 != utf32_string(utf8); }

	friend bool operator!=(char const* utf8, utf32_string const& utf32)
		{ return utf32_string(utf8) != utf32; }

	friend bool operator!=(utf32_string const& utf32, std::string const& utf8)
		{ return utf32 != utf32_string(utf8); }

	friend bool operator!=(std::string const& utf8, utf32_string const& utf32)
		{ return utf32_string(utf8) != utf32; }


	friend bool operator<=(utf32_string const& s1, utf32_string const& s2)
		{ return s1.m_data <= s2.m_data; }

	friend bool operator<=(utf32_string const& utf32, char const* utf8)
		{ return utf32 <= utf32_string(utf8); }

	friend bool operator<=(char const* utf8, utf32_string const& utf32)
		{ return utf32_string(utf8) <= utf32; }

	friend bool operator<=(utf32_string const& utf32, std::string const& utf8)
		{ return utf32 <= utf32_string(utf8); }

	friend bool operator<=(std::string const& utf8, utf32_string const& utf32)
		{ return utf32_string(utf8) <= utf32; }


	friend bool operator>=(utf32_string const& s1, utf32_string const& s2)
		{ return s1.m_data >= s2.m_data; }

	friend bool operator>=(utf32_string const& utf32, char const* utf8)
		{ return utf32 >= utf32_string(utf8); }

	friend bool operator>=(char const* utf8, utf32_string const& utf32)
		{ return utf32_string(utf8) >= utf32; }

	friend bool operator>=(utf32_string const& utf32, std::string const& utf8)
		{ return utf32 >= utf32_string(utf8); }

	friend bool operator>=(std::string const& utf8, utf32_string const& utf32)
		{ return utf32_string(utf8) >= utf32; }


	friend bool operator<(utf32_string const& s1, utf32_string const& s2)
		{ return s1.m_data < s2.m_data; }

	friend bool operator<(utf32_string const& utf32, char const* utf8)
		{ return utf32 < utf32_string(utf8); }

	friend bool operator<(char const* utf8, utf32_string const& utf32)
		{ return utf32_string(utf8) < utf32; }

	friend bool operator<(utf32_string const& utf32, std::string const& utf8)
		{ return utf32 < utf32_string(utf8); }

	friend bool operator<(std::string const& utf8, utf32_string const& utf32)
		{ return utf32_string(utf8) < utf32; }


	friend bool operator>(utf32_string const& s1, utf32_string const& s2)
		{ return s1.m_data > s2.m_data; }

	friend bool operator>(utf32_string const& utf32, char const* utf8)
		{ return utf32 > utf32_string(utf8); }

	friend bool operator>(char const* utf8, utf32_string const& utf32)
		{ return utf32_string(utf8) > utf32; }

	friend bool operator>(utf32_string const& utf32, std::string const& utf8)
		{ return utf32 > utf32_string(utf8); }

	friend bool operator>(std::string const& utf8, utf32_string const& utf32)
		{ return utf32_string(utf8) > utf32; }

	std::string string() const { return utf32_to_utf8(m_data); }
	std::string string(std::size_t n) const
		{ assert(n <= m_data.size()); return utf32_to_utf8(std::u32string(1, m_data[n])); }

	operator std::string() const { return string(); }

	void clear() { m_data.clear(); }
	void release() { std::u32string().swap(m_data); }
	void shrink() { std::u32string(m_data).swap(m_data); }
	void shrink_to_fit() { m_data.shrink_to_fit(); }

	auto begin() { return std::begin(m_data); }
	auto begin() const { return std::begin(m_data); }

	auto end() { return std::end(m_data); }
	auto end() const { return std::end(m_data); }

	auto cbegin() const { return std::cbegin(m_data); }
	auto cend() const { return std::cend(m_data); }

	auto rbegin() { return std::rbegin(m_data); }
	auto rbegin() const { return std::rbegin(m_data); }

	auto rend() { return std::rend(m_data); }
	auto rend() const { return std::rend(m_data); }

	auto crbegin() const { return std::crbegin(m_data); }
	auto crend() const { return std::crend(m_data); }

	friend
	std::ostream& operator<<(std::ostream& os, utf32_string const& s)
		{ return os << s.string(); }

	friend
	std::istream& operator>>(std::istream& is, utf32_string& s)
	{
		std::string utf8;
		if(is >> utf8)
			s = utf8;
		return is;
	}

	auto find_last_of(char32_t c, std::size_t pos = 0) const
		{ return m_data.find_last_of(c, pos); }

	auto substr(std::size_t pos, std::size_t n = npos) const
		{ return m_data.substr(pos, n); }

	auto c_str() const
		{ return m_data.c_str(); }

	auto empty() const
		{ return m_data.empty(); }

	constexpr static std::size_t const npos = std::size_t(-1);

private:
	std::u32string m_data;
};

enum class encoding_type{unknown, utf8, utf16le, utf16be, utf32le, utf32be};

#if __cplusplus >= 201703L
	using byte = std::byte;
#else
	using byte = unsigned char;
#endif

inline
encoding_type get_encoding_from_BOM(byte* buf, std::size_t n)
{
	// utf8	- EF BB BF
	// utf16le - FF FE
	// utf16be - FE FF
	// utf32le - FF FE 00 00
	// utf32be - 00 00 FE FF

	if(n > 1) // UTF-16
	{
		if(buf[0] == byte(0xFF) && buf[1] == byte(0xFE))
			return encoding_type::utf16le;
		else if(buf[0] == byte(0xFE) && buf[1] == byte(0xFF))
			return encoding_type::utf16be;
	}

	if(n > 2) // UTF-8
	{
		if(buf[0] == byte(0xEF) && buf[1] == byte(0xBB) && buf[2] == byte(0xBF))
			return encoding_type::utf8;
	}

	if(n == 4) // UTF-32
	{
		if(buf[0] == byte(0xFF) && buf[1] == byte(0xFE) && buf[2] == byte(0x00) && buf[3] == byte(0x00))
			return encoding_type::utf32le;
		else if(buf[0] == byte(0x00) && buf[1] == byte(0x00) && buf[2] == byte(0xFE) && buf[3] == byte(0xFF))
			return encoding_type::utf32be;
	}

	return encoding_type::unknown;
}

} // unicode_utils
} // header_only_library

#endif // HEADER_ONLY_LIBRARY_UNICODE_UTILS_H
