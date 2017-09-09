#ifndef HOL_SRANGE_H
#define HOL_SRANGE_H
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

#include <algorithm>
#include <iterator>
#include <regex>
#include <utility>
#include <vector>

#include <cassert>

#include "range.h"

namespace header_only_library {
namespace range {

template<typename CharT>
class basic_srange
: public basic_range<CharT>
{
public:
	basic_srange() noexcept: basic_range<CharT>() {}

//	basic_srange(basic_srange const& r) noexcept: basic_range<CharT>(r.data(), r.size()) {}
	basic_srange(basic_srange const& r) noexcept = default;
	basic_srange(basic_srange&& r) noexcept = default;

	basic_srange& operator=(basic_srange const& r) noexcept = default;
	basic_srange& operator=(basic_srange&& r) noexcept = default;

	explicit basic_srange(CharT* beg, CharT* end) noexcept: basic_range<CharT>(beg, end) {}

	explicit basic_srange(CharT* beg, std::size_t len) noexcept: basic_srange(beg, beg + len) {}

	explicit basic_srange(basic_range<CharT> r): basic_srange(r.data(), r.size()) {}


	// This constructor removes the end character (null terminator)
	template<std::size_t Size>
	explicit basic_srange(CharT(&a)[Size]) noexcept: basic_srange(a, a + Size - 1) {}

	template<typename Container>
	explicit basic_srange(Container&& c) noexcept
	: basic_srange(&std::forward<Container>(c)[0], std::forward<Container>(c).size()) {}

// &*beg => UB when beg == end
//	template<typename ForwardIter>
//	explicit basic_srange(ForwardIter beg, ForwardIter end) noexcept: basic_srange(&*beg, std::distance(beg, end)) {}

	basic_srange(CharT* s) noexcept
	: basic_srange(s, detail::length_of(s)) {}

	basic_srange substr(std::size_t pos, std::size_t len) noexcept
	{
		return basic_srange<CharT>(basic_range<CharT>::subrange(pos, len));
	}

	basic_srange substr(std::size_t pos) noexcept
	{
		return basic_srange<CharT>(basic_range<CharT>::subrange(pos));
	}

	std::basic_string<typename std::remove_const<CharT>::type>
	string()
	{
		return {basic_range<CharT>::m_beg, basic_range<CharT>::m_end};
	}

};

namespace detail {

template<typename CharT>
std::size_t strlen(const CharT* s)
{
	auto p = s;
	while(*p) ++p;
	return std::size_t(p - s);
}

} // namespace detail

using srange = basic_srange<char>;
using const_srange = basic_srange<char const>;

using wsrange = basic_srange<wchar_t>;
using const_wsrange = basic_srange<wchar_t const>;

using u16srange = basic_srange<char16_t>;
using const_u16srange = basic_srange<char16_t const>;

using u32srange = basic_srange<char32_t>;
using const_u32srange = basic_srange<char32_t const>;

template<typename String>
auto make_srange(String& c)
	{ return basic_srange<typename String::value_type>(c); }

template<typename String>
auto make_srange(String const& c)
	{ return basic_srange<typename String::value_type const>(c); }

template<typename CharT>
auto make_srange(CharT* s, std::size_t n)
	{ return basic_srange<CharT>(s, n); }

template<typename CharT>
auto make_srange(CharT* s)
	{ return make_srange(s, detail::strlen(s)); }

template<typename CharT>
auto make_srange(CharT const* s, std::size_t n)
	{ return basic_srange<CharT const>(s, n); }

template<typename CharT>
auto make_srange(CharT const* s)
	{ return make_srange(s, detail::strlen(s)); }

template<typename CharT, std::size_t Size>
auto make_srange(CharT(&a)[Size])
	{ return basic_srange<CharT>(a, Size); }

template<typename CharT, std::size_t Size>
auto make_srange(CharT const(&a)[Size])
	{ return basic_srange<CharT const>(a, Size); }

template<typename CharT>
auto make_srange(CharT* beg, CharT* end)
	{ return basic_srange<CharT>(beg, end); }

template<typename CharT>
auto make_srange(CharT const* beg, CharT const* end)
	{ return basic_srange<CharT>(beg, end); }

// TODO move this to srange_regex ?

using rmatch = std::match_results<srange::iterator>;
using wrmatch = std::match_results<wsrange::iterator>;
using u16rmatch = std::match_results<u16srange::iterator>;
using u32rmatch = std::match_results<u32srange::iterator>;

template<typename BiIter, typename Char = typename std::iterator_traits<BiIter>::value_type>
using basic_rregex_iterator = std::regex_iterator<BiIter, Char>;

template<typename BiIter, typename Char = typename std::iterator_traits<BiIter>::value_type>
using basic_rregex_token_iterator = std::regex_token_iterator<BiIter, Char>;

using rregex_iterator = basic_rregex_iterator<srange::iterator>;
using rregex_token_iterator = basic_rregex_token_iterator<srange::iterator>;

namespace detail {

template<typename MatchType>
std::vector<basic_range<typename MatchType::char_type>> convert_matches(MatchType const& m)
{
	std::vector<basic_range<typename MatchType::char_type>> matches;
	matches.reserve(m.size());

	for(auto& sm: m)
		matches.emplace_back(sm.first, sm.second);

	return matches;
}

} // namespace detail

template<typename Char>
std::vector<basic_range<Char>> regex_match(basic_range<Char> s, std::regex const& e,
	std::regex_constants::match_flag_type flags = std::regex_constants::match_default)
{
	std::match_results<typename basic_range<Char>::iterator> m;
	if(!std::regex_match(std::begin(s), std::end(s), m, e, flags))
		return {};

	return detail::convert_matches(m);

//	std::vector<range<Char>> matches;
//	matches.reserve(m.size());
//
//	for(auto& sm: m)
//		matches.emplace_back(sm.first, sm.second);
//
//	return matches;
}

template<typename Char>
std::vector<basic_range<Char>> regex_search(basic_range<Char> s, std::regex const& e,
	std::regex_constants::match_flag_type flags = std::regex_constants::match_default)
{

	std::match_results<typename basic_range<Char>::iterator> m;
	if(!std::regex_search(std::begin(s), std::end(s), m, e, flags))
		return {};

	return detail::convert_matches(m);

//	std::vector<range<Char>> matches;
//	matches.reserve(m.size());
//
//	for(auto& sm: m)
//		matches.emplace_back(sm.first, sm.second);
//
//	return matches;
}

template<typename Char>
std::vector<std::vector<basic_range<Char>>> regex_search_all(basic_range<Char> s, std::regex const& e,
	std::regex_constants::match_flag_type flags = std::regex_constants::match_default)
{

	std::match_results<typename basic_range<Char>::iterator> m;
	if(!std::regex_search(std::begin(s), std::end(s), m, e, flags))
		return {};

	std::vector<std::vector<basic_range<Char>>> matches;

	basic_rregex_iterator<typename basic_range<Char>::iterator> end;
	basic_rregex_iterator<typename basic_range<Char>::iterator> itr(std::begin(s), std::end(s), e, flags);

	for(; itr != end; ++itr)
		matches.emplace_back(detail::convert_matches(*itr));

	return matches;
}

// string functions (std::string members as free functions)

template<typename Char>
Char* c_str(basic_range<Char> r) { return data(r); }

template<typename Char>
int compare(basic_range<Char> r1, basic_range<Char> r2)
{
	auto len = std::min(r1.size(), r2.size());
	return std::char_traits<Char>::compare(r1.data(), r2.data(), len);
}

namespace literals {

auto operator""_sr(char const* s, std::size_t n) noexcept
	{ return const_srange{s, n}; }

auto operator""_wsr(wchar_t const* s, std::size_t n) noexcept
	{ return const_wsrange{s, n}; }

auto operator""_u16sr(char16_t const* s, std::size_t n) noexcept
	{ return const_u16srange{s, n}; }

auto operator""_u32sr(char32_t const* s, std::size_t n) noexcept
	{ return const_u32srange{s, n}; }

} // literals
} // namespace range
} // namespace header_only_library

#endif // HOL_SRANGE_H
