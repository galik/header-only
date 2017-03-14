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

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <numeric>

#include "test.h"
#include <hol/string_utils.h>
#include <hol/macro_exceptions.h>

namespace hol {
	using namespace header_only_library::string_utils;
}

using namespace hol;
using namespace std::literals::string_literals;

bool operator==(std::vector<std::string> const& lhs, svw_vec const& rhs)
{
	if(lhs.size() != rhs.size())
		return false;
	for(auto i = 0U; i < lhs.size(); ++i)
		if(lhs[i] != rhs[i])
			return false;
	return true;
}

bool operator==(svw_vec const& lhs, std::vector<std::string> const& rhs)
{
	return rhs == lhs;
}

std::istream& sgl(std::istream& is, std::string& line, char delim = '\n')
{
	return std::getline(is, line, delim);
}

std::istream& sgl(std::istream&& is, std::string& line, char delim = '\n')
{
	return sgl(is, line, delim);
}

struct test_info
{
	using vec = std::vector<test_info>;
	std::size_t line_number;
	std::string question;
	std::string answer;
	std::string result;
	bool success = false;
	test_info(std::size_t l, str q, str a)
	: line_number(l), question(q), answer(a) {}
};

struct test_group
{
	using vec = std::vector<test_group>;
	std::string remove;
	test_info::vec lefts;
	test_info::vec rights;
	test_info::vec boths;
};

test_group::vec load_tests()
{
	test_group::vec tests;

	std::ifstream ifs("data/test-trim-utils-01.txt");

	str line;
	auto id = 0U;
	str::size_type pos = 0;
	auto line_number = 0U;

	while(std::getline(ifs, line))
	{
		++line_number;

		if((pos = line.find("//")) != str::npos)
			line.erase(pos);

		if(hol::trim_mute(line).empty())
			continue;

//		if(trim_mutator().round(line).empty())
//			continue;

		//		S "{}"
		//		L "{{{}}}" "}}}"
		//		L "{ {} }" " {} }"
		//		R "{{{}}}" "{{{"
		//		R "{ {} }" "{ {} "
		//		B "{{{}}}" ""
		//		B "{ {} }" " {} "

		if(!line.find("S"))
		{
			str s;
			if(!sgl(sgl(std::istringstream(line) >> s, s, '"'), s, '"'))
				hol_throw_exception(std::runtime_error, "bad format line: " << line_number);

			tests.emplace_back();
			tests.back().remove = s;
		}
		else if(!line.find_first_of("LRB"))
		{
			str q, a;
			if(!sgl(sgl(sgl(sgl(std::istringstream(line) >> q, q, '"'), q, '"'), a, '"'), a, '"'))
				hol_throw_exception(std::runtime_error, "bad format line: " << line_number);

			switch(line[0])
			{
				case 'L':
					tests.back().lefts.emplace_back(line_number, q, a);
					break;
				case 'R':
					tests.back().rights.emplace_back(line_number, q, a);
					break;
				case 'B':
					tests.back().boths.emplace_back(line_number, q, a);
					break;
				break;
			}
		}
	}

	return tests;
}

template<typename TrimmerL, typename TrimmerR, typename TrimmerB>
void generic_trimmer_tests(
	str const& name,
	test_group& test,
	TrimmerL trim_left,
	TrimmerR trim_right,
	TrimmerB trim_both)
{
	for(auto& t: test.lefts)
		if((t.result = trim_left(t.question, test.remove)) == t.answer)
			t.success = true;

	for(auto& t: test.rights)
		if((t.result = trim_right(t.question, test.remove)) == t.answer)
			t.success = true;

	for(auto& t: test.boths)
		if((t.result = trim_both(t.question, test.remove)) == t.answer)
			t.success = true;
}

void display_results(str const& name, test_group::vec const& tests)
{
	std::cout << std::boolalpha;

	for(auto const& test: tests)
	{
		std::cout << name << ": '" << test.remove << "'" << '\n';
		for(auto t: test.lefts)
		{
			std::cout << "    : L : '" << t.question << "' -> '" << t.answer << "': " << t.success << '\n';
			if(!t.success)
				std::cout << "    : E : '" << t.result << "'" << '\n';
		}
		for(auto t: test.rights)
		{
			std::cout << "    : R : '" << t.question << "' -> '" << t.answer << "': " << t.success << '\n';
			if(!t.success)
				std::cout << "    : E : '" << t.result << "'" << '\n';
		}
		for(auto t: test.boths)
		{
			std::cout << "    : B : '" << t.question << "' -> '" << t.answer << "': " << t.success << '\n';
			if(!t.success)
				std::cout << "    : E : '" << t.result << "'" << '\n';
		}
	}
}

int main()
{
	using namespace std::literals::string_literals;

	const auto N = 10000000ULL;

	try
	{
		auto tests = load_tests();

		for(auto& test: tests)
			generic_trimmer_tests("mute", test,
				[&](str s, str r)->str{return hol::trim_left_mute(s, r.c_str());},
				[&](str s, str r)->str{return hol::trim_right_mute(s, r.c_str());},
				[&](str s, str r)->str{return hol::trim_mute(s, r.c_str());}
				);

		display_results("mute", tests);

		tests = load_tests();

		for(auto& test: tests)
			generic_trimmer_tests("copy", test,
				[&](str s, str r)->str{return hol::trim_left_copy(s, r.c_str());},
				[&](str s, str r)->str{return hol::trim_right_copy(s, r.c_str());},
				[&](str s, str r)->str{return hol::trim_copy(s, r.c_str());}
				);

		display_results("copy", tests);
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







