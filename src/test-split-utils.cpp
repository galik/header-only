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

#include <iomanip>
#include <iostream>
#include <cstring>
#include <cctype>
#include <algorithm>
#include <regex>

#include "test.h"
//#include "string_utils.h"

using namespace hol;
using namespace std::literals::string_literals;

namespace hol {

using std::string;
using std::vector;
using std::experimental::string_view;

template<typename IterType, typename ComparatorType, typename ContainerType>
void split(IterType pos, IterType done, ComparatorType cmp, ContainerType& v, bool fold = true)
{
	auto end = pos;

	if(fold)
	{
		while((pos = std::find_if_not(end, done, cmp)) != done)
		{
			end = std::find_if(pos, done, cmp);
			if(end > pos)
				v.emplace_back(pos, end - pos);
		}
	}
	else
	{
		while((end = std::find_if(pos, done, cmp)) != done)
		{
			if(end > pos)
				v.emplace_back(pos, end - pos);
			pos = end + 1;
		}

		if(pos != done)
			v.emplace_back(pos, end - pos);
	}
}

template<typename ComparatorType, typename ContainerType>
void split(string_view s, ComparatorType cmp, ContainerType& v, bool fold = true)
{
	split(s.data(), s.data() + s.size(), cmp, v, fold);
}

template<typename ComparatorType>
vector<string> split_copy(string_view s, ComparatorType cmp, bool fold = true)
{
	vector<string> v;
	split(s, cmp, v, fold);
	return v;
}

template<typename ComparatorType>
vector<string_view> split_view(string_view s, ComparatorType cmp, bool fold = true)
{
	vector<string_view> v;
	split(s, cmp, v, fold);
	return v;
}

inline
vector<string> split_copy_at_space(string_view s, bool fold = true)
{
	return split_copy(s, std::ptr_fun<int, int>(std::isspace), fold);
}

inline
vector<string_view> split_view_at_space(string_view s, bool fold = true)
{
	return split_view(s, std::ptr_fun<int, int>(std::isspace), fold);
}

inline
vector<string> split_copy_at_delim(string_view s, char delim, bool fold = true)
{
	return split_copy(s, [delim](char c){return c == delim;}, fold);
}

inline
vector<string_view> split_view_at_delim(string_view s, char delim, bool fold = true)
{
	return split_view(s, [delim](char c){return c == delim;}, fold);
}

inline
vector<string> split_copy_at_delims(string_view s, string_view delims, bool fold = true)
{
	return split_copy(s, [delims](char c){return delims.find(c) != string_view::npos;}, fold);
}

inline
vector<string_view> split_view_at_delims(string_view s, string_view delims, bool fold = true)
{
	return split_view(s, [delims](char c){return delims.find(c) != string_view::npos;}, fold);
}

template<typename ContainerType>
void split_at_delim(string_view s, string_view delim, ContainerType& v)
{
	auto done = s.data() + s.size();
	auto end = s.data();
	auto pos = end;

	while((end = std::search(pos, done, delim.begin(), delim.end())) != done)
	{
		if(end > pos)
			v.emplace_back(pos, end - pos);
		pos = end + delim.size();
	}

	if(pos != done)
		if(end > pos)
			v.emplace_back(pos, end - pos);
}

inline
vector<string> split_copy_at_delim(string_view s, string_view delim)
{
	vector<string> v;
	split_at_delim(s, delim, v);
	return v;
}

inline
vector<string_view> split_view_at_delim(string_view s, string_view delim)
{
	vector<string_view> v;
	split_at_delim(s, delim, v);
	return v;
}

template<typename ContainerType>
void split_at_regex(std::regex e, string_view s, ContainerType& v)
{
	auto pos = s.data();
	auto end = pos + s.size();
	std::cmatch m;

	while(std::regex_search(pos, end, m, e))
	{
		if(!m.empty())
			v.emplace_back(pos, m.position());
		pos = pos + m.position() + m.length();
	}

	if(end > pos)
		v.emplace_back(pos, end - pos);
}

inline
vector<string> split_copy_at_regex(std::regex e, string_view s, std::size_t reserve_guess = 20)
{
	vector<string> v;
	split_at_regex(e, s, v);
	return v;
}

inline
vector<string_view> split_view_at_regex(std::regex e, string_view s, std::size_t reserve_guess = 20)
{
	vector<string_view> v;
	split_at_regex(e, s, v);
	return v;
}

inline
vector<string> split_copy_at_regex(string const& e, string_view s, std::size_t reserve_guess = 20)
{
	vector<string> v;
	split_at_regex(std::regex(e), s, v);
	return v;
}

inline
vector<string_view> split_view_at_regex(string const& e, string_view s, std::size_t reserve_guess = 20)
{
	vector<string_view> v;
	split_at_regex(std::regex(e), s, v);
	return v;
}

} // hol

hol::string get_string(const hol::string& s) { return s; }

struct split_test_item
{
	str s;
	str_vec v;
};

bool operator==(str_vec const& lhs, svw_vec const& rhs)
{
	if(lhs.size() != rhs.size())
		return false;
	for(auto i = 0U; i < lhs.size(); ++i)
		if(lhs[i] != rhs[i])
			return false;
	return true;
}

bool operator==(svw_vec const& lhs, str_vec const& rhs)
{
	return rhs == lhs;
}

int main()
{
//	for(auto const& s: split_copy_at_regex(std::regex(" "), "a b c d"))
//		con("copy: " << s);
//
//	for(auto const& s: split_view_at_regex(std::regex(" "), "a b c d"))
//		con("view: " << s);
//
//	return 0;
//
	try
	{
		str s;

		const std::multimap<char, split_test_item> char_split_folds =
		{
			  {' ', {"", {}}}
			, {' ', {"x", {"x"}}}
			, {' ', {" x", {"x"}}}
			, {' ', {"x ", {"x"}}}
			, {' ', {" x ", {"x"}}}
			, {' ', {"x  ", {"x"}}}
			, {' ', {"  x", {"x"}}}
			, {' ', {"  x  ", {"x"}}}
			, {' ', {"a b", {"a", "b"}}}
			, {' ', {" a b", {"a", "b"}}}
			, {' ', {"a b ", {"a", "b"}}}
			, {' ', {"  a b", {"a", "b"}}}
			, {' ', {"a b  ", {"a", "b"}}}
			, {' ', {"a  b", {"a", "b"}}}
			, {' ', {"aaa bbb", {"aaa", "bbb"}}}
			, {' ', {" aaa bbb", {"aaa", "bbb"}}}
			, {' ', {"aaa bbb ", {"aaa", "bbb"}}}
			, {' ', {"  aaa bbb", {"aaa", "bbb"}}}
			, {' ', {"aaa bbb  ", {"aaa", "bbb"}}}
			, {' ', {"aaa  bbb", {"aaa", "bbb"}}}
		};

		for(auto const& s: char_split_folds)
		{
			con("==== split_copy_at_delim: '" << s.second.s << "', '" << s.first << "'");
			con((s.second.v == split_copy_at_delim(s.second.s, s.first, true)));
//				for(auto const& p: split_copy_at_delim(s.second.s, s.first, fold))
//					con("'" << p << "'");
			con("");
			con("==== split_view_at_delim: '" << s.second.s << "', '" << s.first << "'");
			con((s.second.v == split_view_at_delim(s.second.s, s.first, true)));
//				for(auto const& p: split_view_at_delim(s.second.s, s.first, fold))
//					con("'" << p << "'");
			con("");
		}
	}
	catch(const std::exception& e)
	{
		err(e.what());
	}
	catch(...)
	{
		err("Unknown exception thrown");
	}
}






