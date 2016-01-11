#ifndef GALIK_STRING_VIEW_STREAM_H
#define GALIK_STRING_VIEW_STREAM_H

#include <istream>
#include <experimental/string_view>

namespace galik {

namespace ex = std::experimental;

template<typename CharType>
class basic_string_view_buf
: public std::basic_streambuf<CharType>
{
	using char_type = CharType;
	using buf_type = std::basic_streambuf<char_type>;
	using traits_type = buf_type::traits_type;
	using string_view_type = ex::basic_string_view<char_type>;

	std::locale loc;
	string_view_type sv;

public:
	basic_string_view_buf(string_view_type sv): sv(sv)
	{
		setg(const_cast<char_type*>(sv.data())
			, const_cast<char_type*>(sv.data())
				, const_cast<char_type*>(sv.data()) + sv.size());
	}

	void imbue(const std::locale& loc) override { this->loc = loc; }

	buf_type* setbuf(char_type* b, std::streamsize l) override
	{
		sv = string_view_type(b, l);
		setg(const_cast<char_type*>(sv.data())
			, const_cast<char_type*>(sv.data())
				, const_cast<char_type*>(sv.data()) + sv.size());
		return this;
	}

	pos_type seekoff(off_type off, std::ios::seekdir dir, std::ios::openmode which) override
	{
		if(which == std::ios::in)
		{
			if(dir == std::ios::cur)
				gbump(off);
			else if(dir == std::ios::beg)
				setg(eback(), eback() + off, egptr());
			else if(dir == std::ios::end)
				setg(eback(), egptr() - off, egptr());
		}

		return egptr() - eback();
	}

	pos_type seekpos(pos_type pos, std::ios::openmode which) override
	{
		return seekoff(pos, std::ios::beg, which);
	}

	std::streamsize showmanyc() override
	{
		if(gptr() == egptr())
			return -1;
		return egptr() - gptr();
	}

	std::streamsize xsgetn(char_type* s, std::streamsize n)
	{
		char_type* e = gptr() + n;
		if(e > egptr())
		{
			e = egptr();
			n = e - gptr();
		}
		std::copy(s, s + n, gptr());
		gbump(n);
		return n;
	}
};

template<typename CharType>
class basic_string_view_stream
: public std::basic_istream<CharType>
{
	using char_type = CharType;
	using buf_type = std::basic_streambuf<char_type>;
	using traits_type = buf_type::traits_type;
	using string_view_type = ex::basic_string_view<char_type>;

	basic_string_view_buf<char_type> buf;
public:
	basic_string_view_stream(string_view_type sv): std::istream(&buf), buf(sv) {}
};

using string_view_buf = basic_string_view_buf<char>;
using wstring_view_buf = basic_string_view_buf<wchar_t>;
using string_view_stream = basic_string_view_stream<char>;
using wstring_view_stream = basic_string_view_stream<wchar_t>;

} // galik

#endif // GALIK_STRING_VIEW_STREAM_H
