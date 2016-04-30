#ifndef HOL_STRING_VIEW_STREAM_H
#define HOL_STRING_VIEW_STREAM_H

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

#include <istream>
#include <streambuf>
#include <experimental/string_view>

#include "../hol/bug.h"

namespace hol {

namespace ex = std::experimental;

template<typename CharType>
class basic_string_view_buf
: public std::basic_streambuf<CharType>
{
	using char_type = CharType;
	using buf_type = std::basic_streambuf<char_type>;
	using traits_type = typename buf_type::traits_type;
	using string_view_type = ex::basic_string_view<char_type>;
	using pos_type = typename buf_type::pos_type;
	using off_type = typename buf_type::off_type;

	std::locale loc;
	string_view_type sv;

public:
	basic_string_view_buf(string_view_type sv): sv(sv)
	{
//		bug_fun();
		buf_type::setg(const_cast<char_type*>(sv.data())
			, const_cast<char_type*>(sv.data())
				, const_cast<char_type*>(sv.data()) + sv.size());
	}

	string_view_type get_string_view() const { return sv; }

	void imbue(const std::locale& loc) override { this->loc = loc; }

	buf_type* setbuf(char_type* b, std::streamsize l) override
	{
//		bug_fun();
		sv = string_view_type(b, l);
		buf_type::setg(const_cast<char_type*>(sv.data())
			, const_cast<char_type*>(sv.data())
				, const_cast<char_type*>(sv.data()) + sv.size());
		return this;
	}

	pos_type seekoff(off_type off, std::ios::seekdir dir, std::ios::openmode which) override
	{
//		bug_fun();
		if(which == std::ios::in)
		{
			if(dir == std::ios::cur)
				buf_type::gbump(off);
			else if(dir == std::ios::beg)
				buf_type::setg(buf_type::eback()
					, buf_type::eback() + off
						, buf_type::egptr());
			else if(dir == std::ios::end)
				buf_type::setg(buf_type::eback()
					, buf_type::egptr() - off
						, buf_type::egptr());
		}

		return buf_type::gptr() - buf_type::eback();
	}

	pos_type seekpos(pos_type pos, std::ios::openmode which) override
	{
//		bug_fun();
		return seekoff(pos, std::ios::beg, which);
	}

	std::streamsize showmanyc() override
	{
//		bug_fun();
		if(buf_type::gptr() == buf_type::egptr())
			return -1;
		return buf_type::egptr() - buf_type::gptr();
	}

	std::streamsize xsgetn(char_type* s, std::streamsize n)
	{
//		bug_fun();
		char_type* e = buf_type::gptr() + n;
		if(e > buf_type::egptr())
		{
			e = buf_type::egptr();
			n = e - buf_type::gptr();
		}
		std::copy(s, s + n, buf_type::gptr());
		buf_type::gbump(n);
		return n;
	}
};

template<typename CharType>
class basic_string_view_stream
: public std::basic_istream<CharType>
{
	using char_type = CharType;
	using buf_type = std::basic_streambuf<char_type>;
	using traits_type = typename buf_type::traits_type;
	using string_view_type = ex::basic_string_view<char_type>;

	basic_string_view_buf<char_type> buf;

public:
	basic_string_view_stream(string_view_type sv): std::basic_istream<CharType>(), buf(sv)
	{
		std::basic_istream<CharType>::rdbuf(&buf);
	}

	string_view_type get_string_view() const { return buf.get_string_view(); }

};

using string_view_buf = basic_string_view_buf<char>;
using wstring_view_buf = basic_string_view_buf<wchar_t>;
using string_view_stream = basic_string_view_stream<char>;
using wstring_view_stream = basic_string_view_stream<wchar_t>;

} // hol

template<typename CharType>
hol::basic_string_view_stream<CharType>& operator>>(hol::basic_string_view_stream<CharType>& isv, hol::ex::string_view& sv)
{
	sv = isv.get_string_view();
	auto size = sv.size();
	sv.remove_prefix(isv.tellg());
	isv.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
	sv.remove_suffix(isv.size() - isv.tellg());
	return isv;
}

#endif // HOL_STRING_VIEW_STREAM_H
