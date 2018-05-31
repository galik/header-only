#ifndef HEADER_ONLY_LIBRARY_STRING_CONVERSIONS_H
#define HEADER_ONLY_LIBRARY_STRING_CONVERSIONS_H
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

#include <cstdlib>
#include <string>

// FIXME: std::numeric_limits<Unsigned>::max_digits10 only works for FLOAT!!!

namespace header_only_library {
namespace string_conversions {
namespace detail {

// TODO: test and introduce this Unicode conversion table
template<typename CharT>
char narrowly_convert(CharT c)
{
	char32_t digit_beg = 0x0660;

	constexpr char32_t const digit_ranges[] =
	{
		// start positions for 10-digit Unicode ranges
		// TODO: rearrange these in order of most common to least common
		0x0660, 0x06F0, 0x07C0, 0x0966, 0x09E6, 0x0A66, 0x0AE6, 0x0B66, 0x0BE6,
		0x0C66, 0x0CE6, 0x0D66, 0x0DE6, 0x0E50, 0x0ED0, 0x0F20, 0x1040, 0x1090,
		0x17E0, 0x1810, 0x1946, 0x19D0, 0x1A80, 0x1A90, 0x1B50, 0x1BB0, 0x1C40,
		0x1C50, 0xA620, 0xA8D0, 0xA900, 0xA9D0, 0xA9F0, 0xAA50, 0xABF0, 0xFF10,
		0x104A0, 0x11066, 0x110F0, 0x11136, 0x111D0, 0x112F0, 0x11450, 0x114D0,
		0x11650, 0x116C0, 0x11730, 0x118E0, 0x11C50, 0x11D50, 0x16A60, 0x16B50,
		0x1D7CE, 0x1D7D8, 0x1D7E2, 0x1D7EC, 0x1D7F6, 0x1E950,
	};

	for(auto digit_range: digit_ranges)
		if(c >= digit_range && c < digit_range + 10)
			return char('0' + char(c - digit_range));

	return char(c);
}

template<typename CharT>
CharT const* make_narrow(CharT const* in, char*& out, std::size_t out_size)
{
	auto pos = in;

	while(std::isdigit(char(*pos)) && out_size--)
		*out++ = char(*pos++);

	return pos == in ? nullptr : pos;
}

inline
bool s_to_test(char const* s, char const* e)
{
	if(e == s)
		return false;

	while(std::isspace(*e))
		++e;

	return !(*e);
}

} // namespace detail

/**
 *
 * @param s String to convert.
 * @param i Converted output. ONLY set if conversion succeeds.
 * @param base
 * @return Pointer to first unconverted character or nullptr on failure to convert.
 */
template<typename Signed>
char const* s_to_i(char const* s, Signed& i, int base = 10)
{
	static_assert(std::is_integral<Signed>::value, "s_to_i() requires an integral output value");
	static_assert(std::is_signed<Signed>::value, "s_to_i() requires a signed output value, did you mean s_to_u()?");

	char* end;
	auto l = std::strtoll(s, &end, base);

	if(l > std::numeric_limits<Signed>::max()
	|| l < std::numeric_limits<Signed>::min())
		return nullptr;

	if(!detail::s_to_test(s, end))
		return nullptr;

	i = static_cast<Signed>(l);

	return end;
}

template<typename CharT, typename Signed>
CharT const* s_to_i(CharT const* s, Signed& i, int base = 10)
{
	char buf[std::numeric_limits<Signed>::max_digits10 + 1];
	if(auto* pos = detail::make_narrow(s, buf, sizeof(buf)))
		*pos = '\0';
	else
		return nullptr;

	if(auto* pos = s_to_i(buf, i, base))
		return s + (pos - buf);

	return nullptr;
}

template<typename CharT, typename Traits = std::char_traits<CharT>,
	typename Alloc = std::allocator<CharT>, typename Signed>
bool s_to_i(std::basic_string<CharT, Traits, Alloc> const& s, Signed& i, int base = 10)
{
	return bool(s_to_i(s.c_str(), i, base));
}

template<typename Unsigned>
char const* s_to_u(char const* s, Unsigned& i, int base = 10)
{
	static_assert(std::is_integral<Unsigned>::value, "s_to_u() requires an integral output value");
	static_assert(std::is_unsigned<Unsigned>::value, "s_to_u() requires an unsigned output value, did you mean s_to_i()?");

	char* end;
	auto l = std::strtoull(s, &end, base);

	if(l > std::numeric_limits<Unsigned>::max())
		return nullptr;

	if(!detail::s_to_test(s, end))
		return nullptr;

	i = static_cast<Unsigned>(l);

	return end;
}

template<typename CharT, typename Unsigned>
CharT const* s_to_u(CharT const* s, Unsigned& i, int base = 10)
{
	char buf[std::numeric_limits<Unsigned>::max_digits10 + 1];

	if(auto* pos = detail::make_narrow(s, buf, sizeof(buf)))
		*pos = '\0';
	else
		return nullptr;

	if(auto* pos = s_to_u(buf, i, base))
		return s + (pos - buf);

	return nullptr;
}

template<typename CharT, typename Traits = std::char_traits<CharT>,
	typename Alloc = std::allocator<CharT>, typename Unsigned>
bool s_to_u(std::basic_string<CharT, Traits, Alloc>const& s, Unsigned& i, int base = 10)
{
	return bool(s_to_u(s.c_str(), i, base));
}

inline
char const* s_to_i(char const* s, long int& i)
{
	char* end;
	auto tmp = std::strtol(s, &end, 10);

	if(!detail::s_to_test(s, end))
		return nullptr;

	i = tmp;

	return end;
}

template<typename CharT>
CharT const* s_to_i(CharT const* s, long int& i)
{
	char buf[std::numeric_limits<decltype(i)>::max_digits10 + 1];

	if(auto* pos = detail::make_narrow(s, buf, sizeof(buf)))
		*pos = '\0';
	else
		return nullptr;

	if(auto* pos = s_to_i(buf, i))
		return s + (pos - buf);

	return nullptr;
}

template<typename CharT, typename Traits = std::char_traits<CharT>,
	typename Alloc = std::allocator<CharT>>
bool s_to_i(std::basic_string<CharT, Traits, Alloc>const& s, long int& i)
{
	return s_to_i(s.c_str(), i);
}

inline
char const* s_to_i(char const* s, long long int& i)
{
	char* end;
	auto tmp = std::strtoll(s, &end, 10);

	if(!detail::s_to_test(s, end))
		return nullptr;

	i = tmp;

	return end;
}

template<typename CharT>
CharT const* s_to_i(CharT const* s, long long int& i)
{
	char buf[std::numeric_limits<decltype(i)>::max_digits10 + 1];

	if(auto* pos = detail::make_narrow(s, buf, sizeof(buf)))
		*pos = '\0';
	else
		return nullptr;

	if(auto* pos = s_to_i(buf, i))
		return s + (pos - buf);

	return nullptr;
}

template<typename CharT, typename Traits = std::char_traits<CharT>,
	typename Alloc = std::allocator<CharT>>
bool s_to_i(std::basic_string<CharT, Traits, Alloc>const& s, long long int& i)
{
	return s_to_i(s.c_str(), i);
}

inline
char const* s_to_u(char const* s, unsigned long int& u)
{
	char* end;
	auto tmp = std::strtoul(s, &end, 10);

	if(!detail::s_to_test(s, end))
		return nullptr;

	u = tmp;

	return end;
}

template<typename CharT>
CharT const* s_to_u(CharT const* s, unsigned long int& u)
{
	char buf[std::numeric_limits<decltype(u)>::max_digits10 + 1];

	if(auto* pos = detail::make_narrow(s, buf, sizeof(buf)))
		*pos = '\0';
	else
		return nullptr;

	if(auto* pos = s_to_u(buf, u))
		return s + (pos - buf);

	return nullptr;
}

template<typename CharT, typename Traits = std::char_traits<CharT>,
	typename Alloc = std::allocator<CharT>>
bool s_to_u(std::basic_string<CharT, Traits, Alloc>const& s, unsigned long int& u)
{
	return s_to_u(s.c_str(), u);
}

inline
char const* s_to_u(char const* s, unsigned long long int& u)
{
	char* end;
	auto tmp = std::strtoull(s, &end, 10);

	if(!detail::s_to_test(s, end))
		return nullptr;

	u = tmp;

	return end;
}

template<typename CharT>
CharT const* s_to_u(CharT const* s, unsigned long long int& u)
{
	char buf[std::numeric_limits<decltype(u)>::max_digits10 + 1];

	if(auto* pos = detail::make_narrow(s, buf, sizeof(buf)))
		*pos = '\0';
	else
		return nullptr;

	if(auto* pos = s_to_u(buf, u))
		return s + (pos - buf);

	return nullptr;
}

template<typename CharT, typename Traits = std::char_traits<CharT>,
	typename Alloc = std::allocator<CharT>>
bool s_to_u(std::basic_string<CharT, Traits, Alloc>const& s, unsigned long long int& u)
{
	return s_to_u(s.c_str(), u);
}

// TODO:

inline
char const* s_to_r(char const* s, float& r)
{
	char* end;
	auto tmp = std::strtof(s, &end);

	if(!detail::s_to_test(s, end))
		return nullptr;

	r = tmp;

	return end;
}

template<typename CharT>
CharT const* s_to_r(CharT const* s, float& r)
{
	char buf[std::numeric_limits<decltype(r)>::max_digits10 + 1];

	if(auto* pos = detail::make_narrow(s, buf, sizeof(buf)))
		*pos = '\0';
	else
		return nullptr;

	if(auto* pos = s_to_r(buf, r))
		return s + (pos - buf);

	return nullptr;
}

template<typename CharT, typename Traits = std::char_traits<CharT>,
	typename Alloc = std::allocator<CharT>>
bool s_to_r(std::basic_string<CharT, Traits, Alloc>const& s, float& r)
{
	return bool(s_to_r(s.c_str(), r));
}

inline
char const* s_to_r(char const* s, double& r)
{
	char* end;
	auto tmp = std::strtod(s, &end);

	if(!detail::s_to_test(s, end))
		return nullptr;

	r = tmp;

	return end;
}

template<typename CharT>
CharT const* s_to_r(CharT const* s, double& r)
{
	char buf[std::numeric_limits<decltype(r)>::max_digits10 + 1];

	if(auto* pos = detail::make_narrow(s, buf, sizeof(buf)))
		*pos = '\0';
	else
		return nullptr;

	if(auto* pos = s_to_r(buf, r))
		return s + (pos - buf);

	return nullptr;
}

template<typename CharT, typename Traits = std::char_traits<CharT>,
	typename Alloc = std::allocator<CharT>>
bool s_to_r(std::basic_string<CharT, Traits, Alloc>const& s, double& r)
{
	return bool(s_to_r(s.c_str(), r));
}

inline
char const* s_to_r(char const* s, long double& r)
{
	char* end;
	auto tmp = std::strtold(s, &end);

	if(!detail::s_to_test(s, end))
		return nullptr;

	r = tmp;

	return end;
}

template<typename CharT>
CharT const* s_to_r(CharT const* s, long double& r)
{
	char buf[std::numeric_limits<decltype(r)>::max_digits10 + 1];

	if(auto* pos = detail::make_narrow(s, buf, sizeof(buf)))
		*pos = '\0';
	else
		return nullptr;

	if(auto* pos = s_to_r(buf, r))
		return s + (pos - buf);

	return nullptr;
}

template<typename CharT, typename Traits = std::char_traits<CharT>,
	typename Alloc = std::allocator<CharT>>
bool s_to_r(std::basic_string<CharT, Traits, Alloc>const& s, long double& r)
{
	return bool(s_to_r(s.c_str(), r));
}

} // namespace string_conversions
} // namespace header_only_library

#endif // HEADER_ONLY_LIBRARY_STRING_CONVERSIONS_H
