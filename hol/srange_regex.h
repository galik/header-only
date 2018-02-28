#ifndef HEADER_ONLY_LIBRARY_SRANGE_REGEX_H
#define HEADER_ONLY_LIBRARY_SRANGE_REGEX_H
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

#include "srange.h"

namespace header_only_library {
namespace range_regex {

using namespace range;

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

} // namespace range_regex
} // namespace header_only_library

#endif // HEADER_ONLY_LIBRARY_SRANGE_REGEX_H
