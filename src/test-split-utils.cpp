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

#include <regex>
#include <cctype>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>
#include <algorithm>

#include "test.h"
#include <hol/experimental/utility_trimmers.h>
#include <hol/experimental/utility_splitters.h>

using namespace hol;
using namespace std::literals::string_literals;

namespace hol {


} // hol

std::string get_string(const std::string& s) { return s; }

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
//		OUT("copy: " << s);
//
//	for(auto const& s: split_view_at_regex(std::regex(" "), "a b c d"))
//		OUT("view: " << s);
//
//	return 0;

	try
	{
//		str s;
//
//		const std::multimap<char, split_test_item> char_split_folds =
//		{
//			  {' ', {"", {}}}
//			, {' ', {"x", {"x"}}}
//			, {' ', {" x", {"x"}}}
//			, {' ', {"x ", {"x"}}}
//			, {' ', {" x ", {"x"}}}
//			, {' ', {"x  ", {"x"}}}
//			, {' ', {"  x", {"x"}}}
//			, {' ', {"  x  ", {"x"}}}
//			, {' ', {"a b", {"a", "b"}}}
//			, {' ', {" a b", {"a", "b"}}}
//			, {' ', {"a b ", {"a", "b"}}}
//			, {' ', {"  a b", {"a", "b"}}}
//			, {' ', {"a b  ", {"a", "b"}}}
//			, {' ', {"a  b", {"a", "b"}}}
//			, {' ', {"aaa bbb", {"aaa", "bbb"}}}
//			, {' ', {" aaa bbb", {"aaa", "bbb"}}}
//			, {' ', {"aaa bbb ", {"aaa", "bbb"}}}
//			, {' ', {"  aaa bbb", {"aaa", "bbb"}}}
//			, {' ', {"aaa bbb  ", {"aaa", "bbb"}}}
//			, {' ', {"aaa  bbb", {"aaa", "bbb"}}}
//		};

		trim_mutator trim_mute;


		std::ifstream ifs;

		ifs.open("data/test-split-utils-01.txt");

		str line;
		auto line_number = 0U;

		while(std::getline(ifs, line))
		{
			++line_number;

			if(trim_mute.round(line).empty())
				continue;

			if(line.find("delim:"))
				continue;

			// delim:
			// delim: ' ' "aaa bbb" {"aaa" "bbb"}
			char delim;
			str s;
			str_vec correct;

			std::istringstream iss(line);
			std::getline(iss, line, '\'');
			iss.get(delim);
			std::getline(iss, line, '"');
			std::getline(iss, s, '"');
			std::getline(iss, line, '{');
			std::getline(iss, line, '}');

			if(!iss)
				hol_throw_runtime_error("line: " << line_number << " error parsing delim:");

			iss.clear();
			iss.str(line);
			while(std::getline(iss, line, '"') && std::getline(iss, line, '"'))
				correct.push_back(line);

			OUT("== folding ==");

			split_copier split_copy;
			OUT_B("split_copy.at_delim: '" << s << "', '" << delim << "' | ");
			OUT_E((correct == split_copy.at_delim(s, delim)));

			split_viewer split_view;
			OUT_B("split_view.at_delim: '" << s << "', '" << delim << "' | ");
			OUT_E((correct == split_view.at_delim(s, delim)));

			OUT("== not folding ==");

			split_copy.dont_fold_matches();
			split_view.dont_fold_matches();

			OUT_B("split_copy.at_delim: '" << s << "', '" << delim << "' | ");
			OUT_E((correct == split_copy.at_delim(s, delim)));

			OUT_B("split_view.at_delim: '" << s << "', '" << delim << "' | ");
			OUT_E((correct == split_view.at_delim(s, delim)));
		}

	}
	catch(const std::exception& e)
	{
		ERR(e.what());
	}
	catch(...)
	{
		ERR("Unknown exception thrown");
	}
}







