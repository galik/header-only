#ifndef HOL_STRING_SPAN_UTILS_H
#define HOL_STRING_SPAN_UTILS_H
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

#include <algorithm>
#include <cassert>
#include <string>
#include <vector>

#include <gsl/string_span>

namespace header_only_library {
namespace string_span_utils {

using offset_type = std::ptrdiff_t;

template<typename CharT>
gsl::basic_string_span<CharT> substr(
	gsl::basic_string_span<CharT> s,
	offset_type offset,
	offset_type count)
{
	assert(offset >= 0);
	assert(offset <= s.size());

	return s.subspan(offset, count);
}

template<typename Char1, typename Char2>
offset_type find(
	gsl::basic_string_span<Char1> s,
	Char2 c,
	offset_type offset = 0)
{
	assert(offset >= 0);
	assert(offset <= s.size());

	for(; offset < s.size(); ++offset)
		if(s[offset] == c)
			return offset;

	return s.size();
}

template<typename Char1, typename Char2>
offset_type find_first_of(
	gsl::basic_string_span<Char1> s,
	gsl::basic_string_span<Char2> r,
	offset_type offset = 0)
{
	assert(offset >= 0);
	assert(offset <= s.size());

	for(; offset < s.size(); ++offset)
		if(find(r, s[offset]) != r.size())
			return offset;

	return s.size();
}

template<typename Char1, typename Char2>
offset_type find_first_not_of(
	gsl::basic_string_span<Char1> s,
	gsl::basic_string_span<Char2> r,
	offset_type offset = 0)
{
	assert(offset >= 0);
	assert(offset <= s.size());

	for(; offset < s.size(); ++offset)
		if(find(r, s[offset]) == r.size())
			return offset;

	return s.size();
}

template<typename Char1, typename Char2>
offset_type find_last_of(
	gsl::basic_string_span<Char1> s,
	gsl::basic_string_span<Char2> r,
	offset_type offset = -1)
{
	assert(offset >= -1);
	assert(offset <= s.size());

	if(s.empty())
		return 0;

	for(offset = offset < 0 ? s.size():offset + 1; offset; --offset)
		if(find(r, s[offset - 1]) != r.size())
			return offset - 1;

	return s.size();
}

template<typename Char1, typename Char2>
offset_type find_last_not_of(
	gsl::basic_string_span<Char1> s,
	gsl::basic_string_span<Char2> r,
	offset_type offset = -1)
{
	assert(offset >= -1);
	assert(offset <= s.size());

	if(s.empty())
		return 0;

	for(offset = offset < 0 ? s.size():offset + 1; offset; --offset)
		if(find(r, s[offset - 1]) == r.size())
			return offset - 1;

	return s.size();
}

// Nowhere else to put this yet

//template<typename CharT, typename Traits = std::char_traits<CharT>>
//class basic_char_span_istream;
//
//template<typename CharT, typename Traits = std::char_traits<CharT>>
//class basic_char_span_buf
//: public std::basic_streambuf<CharT, Traits>
//{
//	friend class basic_char_span_istream<CharT, Traits>;
//
//public:
//	using stream_buf = std::basic_streambuf<CharT, Traits>;
//
//	using int_type = typename stream_buf::int_type;
//
//	basic_char_span_buf(gsl::span<CharT> sp): sp(sp)
//		{ stream_buf::setg(sp.data(), sp.data(), sp.data() + sp.size()); }
//
//protected:
//	using stream_buf::eback;
//	using stream_buf::gptr;
//	using stream_buf::egptr;
//	using stream_buf::gbump;
//
//	int_type underflow() override
//	{
//		if(gptr() < egptr())
//			return *gptr();
//		return EOF;
//	}
//
//private:
//	gsl::span<CharT> _getline(CharT delim)
//	{
//		auto pos = std::find(gptr(), egptr(), delim);
//		gsl::span<char> line(gptr(), pos);
//		if(pos < egptr())
//			++pos; // skip delim
//
//		gbump(int(pos - gptr()));
//		return line;
//	}
//
//	gsl::span<CharT> sp;
//};
//
//template<typename CharT, typename Traits = std::char_traits<CharT>>
//basic_char_span_istream<CharT, Traits>&
//getline(basic_char_span_istream<CharT, Traits>& is, gsl::span<CharT>& sp, CharT delim);
//
//template<typename CharT, typename Traits>
//class basic_char_span_istream
//: public std::basic_istream<CharT, Traits>
//{
//	friend basic_char_span_istream& getline<CharT,Traits>(basic_char_span_istream& is, gsl::span<CharT>& sp, CharT delim);
//
//public:
//	basic_char_span_istream(gsl::span<CharT> sp): std::istream(&buf), buf(sp) {}
//
//private:
//	using istream = std::basic_istream<CharT, Traits>;
//
//	gsl::span<CharT> _getline(CharT delim)
//	{
//		if(std::basic_istream<CharT, Traits>::eof())
//		{
//			istream::setstate(istream::failbit);
//			return {};
//		}
//		gsl::span<CharT> line = buf._getline(delim);
//		if(buf.gptr() == buf.egptr())
//			istream::setstate(istream::eofbit);
//		return line;
//	}
//	basic_char_span_buf<CharT, Traits> buf;
//};
//
//using char_span_istream = basic_char_span_istream<char>;
//using wchar_span_istream = basic_char_span_istream<wchar_t>;
//using char16_span_istream = basic_char_span_istream<char16_t>;
//using char32_span_istream = basic_char_span_istream<char32_t>;
//
//template<typename CharT, typename Traits = std::char_traits<CharT>>
//basic_char_span_istream<CharT, Traits>&
//getline(basic_char_span_istream<CharT, Traits>& is, gsl::span<CharT>& sp, CharT delim)
//{
//	sp = is._getline(delim);
//	return is;
//}
//
//char_span_istream&
//getline(char_span_istream& is, gsl::span<char>& sp, char delim = '\n')
//{
//	return getline<char>(is, sp, delim);
//}

// Trimming

template<typename CharT>
using string_span = typename gsl::basic_string_span<CharT>;

template<typename CharT>
using cstring_span = typename gsl::basic_string_span<CharT const>;

template<typename CharT>
using index_type = typename string_span<CharT>::index_type;

namespace detail {
constexpr cstring_span<char>     ws(char)     { return cstring_span<char>    ( " \t\n\r\f\v\0"); }
constexpr cstring_span<wchar_t>  ws(wchar_t)  { return cstring_span<wchar_t> (L" \t\n\r\f\v\0"); }
constexpr cstring_span<char16_t> ws(char16_t) { return cstring_span<char16_t>(u" \t\n\r\f\v\0"); }
constexpr cstring_span<char32_t> ws(char32_t) { return cstring_span<char32_t>(U" \t\n\r\f\v\0"); }

// The following functions have different semantics to the
// standard library ones but they are more efficient for trimming
// so they are in `detail` namespace.

template<typename SpanType, typename CSpanType>
typename SpanType::index_type find_first_not_of(SpanType s, CSpanType t)
{
	for(typename SpanType::index_type i = 0; i < s.size(); ++i)
		if(std::find(t.begin(), t.end(), s[i]) == t.end())
			return i;

	return -1; // 0?
}

template<typename SpanType, typename CSpanType>
typename SpanType::index_type find_last_not_of(SpanType s, CSpanType t)
{
	for(typename SpanType::index_type i = s.size(); i; --i)
		if(std::find(t.begin(), t.end(), s[i - 1]) == t.end())
			return i - 1;

	return -1; // s.size() ?
}

} // detail

template<typename SpanType, typename CSpanType>
SpanType trim_left_span(SpanType s, CSpanType t = detail::ws(typename CSpanType::element_type()))
{
	auto found = detail::find_first_not_of(s, t);

	if(found == -1)
		return {};

	return s.subspan(found);
}

template<typename SpanType, typename CSpanType>
SpanType trim_right_span(SpanType s, CSpanType t = detail::ws(typename CSpanType::element_type()))
{
	return s.subspan(0, detail::find_last_not_of(s, t) + 1);
}

template<typename SpanType, typename CSpanType>
SpanType trim_span(SpanType s, CSpanType t = detail::ws(typename CSpanType::element_type()))
{
	return trim_left_span(trim_right_span(s, t), t);
}

// Splitting

template<typename SpanType, typename CSpanType>
std::vector<SpanType> split_span(SpanType s, CSpanType t)
{
	std::vector<SpanType> v;

	auto beg = s.data();
	auto const end = s.data() + s.size();
	decltype(beg) pos;

	while((pos = std::search(beg, end, t.data(), t.data() + t.size())) != end)
	{
		v.emplace_back(beg, pos);
		beg = pos + t.size();
	}

	if(!v.empty())
		v.emplace_back(beg, pos);

	return v;
}

} // string_span_utils
} // header_only_library

#endif // HOL_STRING_SPAN_UTILS_H
