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
