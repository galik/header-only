#ifndef HEADER_ONLY_LIBRARY_STRING_TOKENIZER_H
#define HEADER_ONLY_LIBRARY_STRING_TOKENIZER_H
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

#include <string>
#include <string_view>

namespace header_only_library {
namespace string_utils {

template<typename CharT, typename StringT = std::basic_string<CharT>>
class basic_string_tokenizer
{
public:
	using string_type = StringT;
	using string_view = std::basic_string_view<CharT>;
	using size_type = typename string_type::size_type;

	basic_string_tokenizer(string_type const& s, string_type const& delims = string_type(1, CharT(' ')))
		: m_s(s), m_delims(delims), m_pos(m_s.find_first_not_of(m_delims)) {}

	friend
	basic_string_tokenizer& operator>>(basic_string_tokenizer& st, string_view& s)
		{ return st.next(s, st.m_delims.c_str()); }

	friend
	basic_string_tokenizer& operator>>(basic_string_tokenizer& st, string_type& s)
	{
		string_view sv;
		if(st >> sv)
			s = sv.to_string();
		return st;
	}

	basic_string_tokenizer& next(string_view& sv)
		{ return next(sv, m_delims.c_str()); }

	basic_string_tokenizer& next(string_view& sv, string_type const& delims)
		{ return next(sv, delims.c_str()); }

	basic_string_tokenizer& next(string_view& sv, CharT const* delims)
	{
		if(eot())
		{
			done = true;
			return *this;
		}

		auto end = m_s.find_first_of(delims, m_pos);

		if(end == string_type::npos)
			end = m_s.size();

		sv = string_view(m_s.data() + m_pos, end - m_pos);

		m_pos = m_s.find_first_not_of(delims, end);

		return *this;
	}

	basic_string_tokenizer& next(string_type& s)
		{ return next(s, m_delims.c_str()); }

	basic_string_tokenizer& next(string_type& s, std::string const& delims)
		{ return next(s, delims.c_str()); }

	basic_string_tokenizer& next(string_type& s, CharT const* delims)
	{
		string_view sv;
		if(next(sv, delims))
			s = sv.to_string();
		return *this;
	}

	bool eot() const { return m_pos == string_type::npos; }

	explicit operator bool() const { return !done; }

	void rewind() { m_pos = 0; done = false; }

private:
	string_type const& m_s;
	string_type const m_delims;
	size_type m_pos;
	bool done = false;
};

using string_tokenizer = basic_string_tokenizer<char>;
using wstring_tokenizer = basic_string_tokenizer<wchar_t>;
using u16string_tokenizer = basic_string_tokenizer<char16_t>;
using u32string_tokenizer = basic_string_tokenizer<char32_t>;

} // namespace string_utils
} // namespace header_only_library

#endif // HEADER_ONLY_LIBRARY_STRING_TOKENIZER_H
