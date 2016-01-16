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

#include "test.h"
#include "string_utils.h"

using namespace galik;
using namespace std::literals::string_literals;

const str_map test_data =
{
	{"", ""}
	, {"x", "x"}
	, {" x", "x"}
	, {"x ", "x"}
	, {" x ", "x"}
	, {"  x  ", "x"}
	, {"a b", "a b"}
	, {" a b", "a b"}
	, {"a b ", "a b"}
	, {" a b ", "a b"}
};

std::string get_string(const std::string& s) { return s; }

//std::vector<string_view> split(const std::string& s, char delim = ' ', bool fold = true)
//{
//	std::vector<string_view> views;
//
//	decltype(s.begin()) i;
//
//	while((i = std::find(s.begin(), s.end(), delim)) != s.end())
//	{
//
//	}
//
//	return views;
//}
//
//std::vector<std::string> split2(const std::string& s)
//{
//	std::vector<std::string> v;
//
//    auto done = s.end();
//    auto end = s.begin();
//    decltype(end) pos;
//
//    while((pos = std::find_if(end, done, std::not1(std::ptr_fun(isspace)))) != done)
//    {
//        end = std::find_if(pos, done, std::ptr_fun(isspace));
//        v.emplace_back(pos, end);
//    }
//    return v;
//}

int main()
{
	try
	{
		std::string s;

		for(auto const& test: test_data)
		{
			s = test.first;

			if(trim_mute(s) != test.second)
				err("fail: trim_mute(): '" << s << "'");
			if(trim_copy(test.first) != test.second)
				err("fail: trim_copy(): '" << trim_copy(test.first) << "'");
			if(trim_view(test.first) != test.second)
				err("fail: trim_view(): '" << trim_view(test.first) << "'");

			if(trim_copy(get_string(test.first)) != test.second)
				err("fail: trim_copy(): '" << trim_copy(test.first) << "'");
			if(trim_view(get_string(test.first)) != test.second)
				err("fail: trim_view(): '" << trim_view(test.first) << "'");
		}

		con("split");

		const str_vec splits =
		{
			  "x"
			, " x"
			, "x "
			, " x "
			, "ab"
			, " ab"
			, "ab "
			, " ab "
			, "1 2 3"
			, " 1 2 3"
			, "1 2 3 "
			, " 1 2 3 "
			, "x  "
		};

		for(auto const& s: splits)
		{
			con("==== split: '" << s << "'");
//			auto v = split_at_spaces(string_view(s), false);
			auto v = split_at_delim(string_view(s), string_view(" "));
			for(auto const& sv: v)
				con("'" << sv << "'");
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






