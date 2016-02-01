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

using namespace hol;
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
//			con("==== split: '" << s << "' (string_view)");
//			for(auto const& sv: split_at_delim(string_view(s), string_view(" ")))
//				con("'" << sv << "'");
//			con("");
//			con("==== split: '" << s << "' (string)");
//			for(auto const& sv: split_at_delim(s, string_view(" ")))
//				con("'" << sv << "'");
//			con("");
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






