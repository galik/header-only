#ifndef HOL_SRANGE_UTILS_H
#define HOL_SRANGE_UTILS_H
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

#include <cstdlib>
#include <vector>

#include "srange.h"
#include "split_algos.h"

namespace header_only_library {
namespace range {

// trimming

namespace detail {

constexpr auto const& ws(char) { return " \t\n\r\f\v\0"; }
constexpr auto const& ws(wchar_t) { return L" \t\n\r\f\v\0"; }
constexpr auto const& ws(char16_t) { return u" \t\n\r\f\v\0"; }
constexpr auto const& ws(char32_t) { return U" \t\n\r\f\v\0"; }

} // namespace detail

template<typename Char>
basic_range<Char> trim_left(basic_range<Char> r, basic_range<Char> ws = detail::ws(Char()))
{
	auto pos = std::find_if_not(std::begin(r), std::end(r), [&](auto c)
		{ return std::find(std::begin(ws), std::end(ws), c) != std::end(ws); });

	return make_range(pos, std::end(r));
}

template<typename Char>
basic_range<Char> trim_right(basic_range<Char> r, basic_range<Char> ws = detail::ws(Char()))
{
	auto rpos = std::find_if_not(std::rbegin(r), std::rend(r), [&](auto c)
		{ return std::find(std::begin(ws), std::end(ws), c) != std::end(ws); });

	return make_range(std::begin(r), rpos.base());
}

template<typename Char>
basic_range<Char> trim(basic_range<Char> r, basic_range<Char> ws = detail::ws(Char()))
{
	return trim_left(trim_right(r, ws), ws);
}

// Number conversions

inline
int stoi(basic_range<char> r, std::size_t* pos = 0, int base = 10)
{
	auto ptr = r.data();
	auto i = std::strtol(r.data(), &ptr, base);

	if(ptr == r.data())
		throw std::invalid_argument("invalid argument: stoi(" + std::string(std::begin(r), std::end(r)) + ")");

	if(i > std::numeric_limits<int>::max()
	|| i < std::numeric_limits<int>::min())
		throw std::out_of_range("out of range: stoi(" + std::string(std::begin(r), std::end(r)) + ")");

	if(pos)
		*pos = std::size_t(std::distance(r.data(), ptr));

	return int(i);
}

inline
long stol(basic_range<char> r, std::size_t* pos = 0, int base = 10)
{
	auto ptr = r.data();
	auto i = std::strtoll(r.data(), &ptr, base);

	if(ptr == r.data())
		throw std::invalid_argument("invalid argument: stol(" + std::string(std::begin(r), std::end(r)) + ")");

	if(i > std::numeric_limits<long>::max()
	|| i < std::numeric_limits<long>::min())
		throw std::out_of_range("out of range: stol(" + std::string(std::begin(r), std::end(r)) + ")");

	if(pos)
		*pos = std::size_t(std::distance(r.data(), ptr));

	return int(i);
}

inline
long long stoll(basic_range<char> r, std::size_t* pos = 0, int base = 10)
{
	auto ptr = r.data();
	auto i = std::strtoll(r.data(), &ptr, base);

	if(ptr == r.data())
		throw std::invalid_argument("invalid argument: stoll(" + std::string(std::begin(r), std::end(r)) + ")");

	if(errno == ERANGE)
		throw std::out_of_range("out of range: stoll(" + std::string(std::begin(r), std::end(r)) + ")");

	if(pos)
		*pos = std::size_t(std::distance(r.data(), ptr));

	return int(i);
}

// finding

namespace detail {

inline
std::size_t strlen(char const* s) { return std::strlen(s); }

//template<typename CharT>
//std::size_t strlen(CharT const* s)
//{
//	auto p = s;
//	while(*p) ++p;
//	return std::size_t(p - s);
//}

template<typename CharT>
std::size_t size(CharT const* s) { return strlen(s); }

template<typename CharT>
std::size_t size(basic_srange<CharT> const& s) { return s.size(); }

template<typename CharT>
auto arg(CharT* s) { return basic_srange<CharT>(s); } //{ return make_srange(s); }

template<typename CharT>
auto arg(basic_srange<CharT> s) { return s; }

} // namespace detail

template<typename SRange1, typename SRange2>
decltype(auto) search(SRange1 r1, SRange2 r2)
{
	auto a = detail::arg(r1);
	auto b = detail::arg(r2);
	return std::search(std::begin(a), std::end(a), std::begin(b), std::end(b));
}

template<typename SRange1, typename SRange2, typename BinaryPredicate>
decltype(auto) search(SRange1 r1, SRange2 r2, BinaryPredicate p)
{
	auto a = detail::arg(r1);
	auto b = detail::arg(r2);
	return std::search(std::begin(a), std::end(a), std::begin(b), std::end(b), p);
}

template<typename CharT>
auto splice_at(basic_srange<CharT> r, typename basic_srange<CharT>::iterator i)
{
	return std::make_pair(basic_srange<CharT>(std::begin(r), i), basic_srange<CharT>(i, std::end(r)));
}

template<typename CharT>
auto splice_to(basic_srange<CharT> s, typename basic_srange<CharT>::iterator i)
	{ return basic_srange<CharT>(std::begin(s), i); }

template<typename CharT>
basic_srange<CharT> splice_from(basic_srange<CharT> s, typename basic_srange<CharT>::iterator i)
	{ return basic_srange<CharT>(i, std::end(s)); }


template<typename CharT, typename Delim1, typename Delim2>
basic_srange<CharT> extract_delimited_text(
	basic_srange<CharT> s,
	Delim1 const& d1,
	Delim2 const& d2,
	basic_srange<CharT>& out)
{
	typename basic_srange<CharT>::iterator beg = search(s, d1);

	if(beg != std::end(s))
	{
		beg = std::next(beg, detail::size(d1));
		s = splice_from(s, beg);
		auto end = search(s, d2);

		if(end != std::end(s))
		{
			out = splice_to(s, end);
			end = std::next(end, detail::size(d2));
			return splice_from(s, end);
		}
	}

	return std::end(s);
}

// Splitting

template<typename CharT>
std::vector<basic_srange<CharT>> split(
	basic_srange<CharT> s,
		basic_srange<CharT const> t = make_srange(algorithm::chr::space(CharT())))
{
	std::vector<basic_srange<CharT>> v;
	algorithm::split(std::begin(s), std::end(s), std::begin(t), std::end(t), algorithm::inserter(v));
	return v;
}

template<typename CharT, std::size_t N>
std::vector<basic_srange<CharT>> split(
	basic_srange<CharT> s,
		CharT const(&t)[N])
			{ return split(s, basic_srange<CharT const>(t)); }

template<typename CharT>
std::vector<basic_srange<CharT>> split_fold(
	basic_srange<CharT> s,
		basic_srange<CharT const> t = make_srange(algorithm::chr::space(CharT())))
{
	std::vector<basic_srange<CharT>> v;
	algorithm::split_fold(std::begin(s), std::end(s), std::begin(t), std::end(t), algorithm::inserter(v));
	return v;
}

template<typename CharT, std::size_t N>
std::vector<basic_srange<CharT>> split_fold(
	basic_srange<CharT> s,
		CharT const(&t)[N])
			{ return split_fold(s, basic_srange<CharT const>(t)); }

} // namespace range_utils
} // namespace header_only_library

#endif // HOL_SRANGE_UTILS_H
