#ifndef HEADER_ONLY_LIBRARY_STRING_SPAN_ISTREAM_H
#define HEADER_ONLY_LIBRARY_STRING_SPAN_ISTREAM_H
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

namespace header_only_library {
namespace string_span_utils {
namespace detail {
bool isspace(char c) { return std::isspace(int(c)); }
bool isspace(wchar_t c) { return std::iswspace(wint_t(c)); }
bool isspace(char16_t c)
{
	constexpr const char16_t* ws = u" \n\t\r";
	constexpr const char16_t* wse = ws + sizeof(ws)/sizeof(ws[0]);
	return std::find(ws, wse , c) != wse;
}
bool isspace(char32_t c)
{
	constexpr const char32_t* ws = U" \n\t\r";
	constexpr const char32_t* wse = ws + sizeof(ws)/sizeof(ws[0]);
	return std::find(ws, wse , c) != wse;
}
} // namespace detail

template<typename CharT, typename Traits = std::char_traits<CharT>>
class basic_string_span_istream;

template<typename CharT, typename Traits = std::char_traits<CharT>>
class basic_string_span_buf
: public std::basic_streambuf<CharT, Traits>
{
	friend class basic_string_span_istream<CharT, Traits>;

public:
	using stream_buf = std::basic_streambuf<CharT, Traits>;
	using span_type = gsl::basic_string_span<CharT>;
	using int_type = typename stream_buf::int_type;

	basic_string_span_buf(span_type sp)
		{ stream_buf::setg(sp.data(), sp.data(), sp.data() + sp.size()); }

protected:
	using stream_buf::eback;
	using stream_buf::gptr;
	using stream_buf::egptr;
	using stream_buf::gbump;

	int_type underflow() override
	{
		if(gptr() < egptr())
			return *gptr();
		return EOF;
	}

private:
	span_type _getline(CharT delim)
	{
		auto pos = std::find(gptr(), egptr(), delim);

		span_type line(gptr(), pos);

		if(pos < egptr())
			++pos; // skip delim

		gbump(int(pos - gptr()));

		return line;
	}

	span_type _extract()
	{
		auto pos = std::find_if(gptr(), egptr(), [](CharT c){ return !detail::isspace(c); });
		gbump(int(pos - gptr()));

		pos = std::find_if(gptr(), egptr(), [](CharT c){ return detail::isspace(c); });
		span_type word(gptr(), pos);
		gbump(int(pos - gptr()));

		return word;
	}
};

template<typename CharT, typename Traits = std::char_traits<CharT>>
basic_string_span_istream<CharT, Traits>&
getline(basic_string_span_istream<CharT, Traits>& is,
	typename basic_string_span_buf<CharT, Traits>::span_type& sp, CharT delim);

template<typename CharT, typename Traits>
class basic_string_span_istream
: public std::basic_istream<CharT, Traits>
{
	friend basic_string_span_istream& getline<CharT,Traits>(basic_string_span_istream& is,
		typename basic_string_span_buf<CharT, Traits>::span_type& sp, CharT delim);

public:
	using span_type = typename basic_string_span_buf<CharT, Traits>::span_type;

	basic_string_span_istream(span_type sp): std::istream(&buf), buf(sp) {}

	friend
	basic_string_span_istream& operator>>(basic_string_span_istream& is, span_type& sp)
	{
		sp = is._extract();
		return is;
	}

private:
	using istream = std::basic_istream<CharT, Traits>;

//	span_type& span() { return buf.sp; }

	span_type _getline(CharT delim)
	{
		if(std::basic_istream<CharT, Traits>::eof())
		{
			istream::setstate(istream::failbit);
			return {};
		}

		span_type line = buf._getline(delim);

		if(buf.gptr() == buf.egptr())
			istream::setstate(istream::eofbit);

		return line;
	}

	span_type _extract()
	{
		if(std::basic_istream<CharT, Traits>::eof())
		{
			istream::setstate(istream::failbit);
			return {};
		}

		span_type word = buf._extract();

		if(buf.gptr() == buf.egptr())
			istream::setstate(istream::eofbit);

		return word;
	}

	basic_string_span_buf<CharT, Traits> buf;
};

using string_span_istream = basic_string_span_istream<char>;
using wstring_span_istream = basic_string_span_istream<wchar_t>;
using u16string_span_istream = basic_string_span_istream<char16_t>;
using u32string_span_istream = basic_string_span_istream<char32_t>;

template<typename CharT, typename Traits = std::char_traits<CharT>>
basic_string_span_istream<CharT, Traits>&
getline(basic_string_span_istream<CharT, Traits>& is,
	typename basic_string_span_buf<CharT, Traits>::span_type& sp, CharT delim)
{
	sp = is._getline(delim);
	return is;
}

string_span_istream&
getline(string_span_istream& is, basic_string_span_buf<char>::span_type& sp, char delim = '\n')
{
	return getline<char>(is, sp, delim);
}

} // namespace string_span_utils
} // namespace header_only_library

#endif // HEADER_ONLY_LIBRARY_STRING_SPAN_ISTREAM_H

