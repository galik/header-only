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

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

//#include <algorithm>
#include <string>
//#include <vector>

#include <hol/bug.h>
#include <hol/byte_utils.h>

namespace bio {
	using namespace header_only_library::byte_utils;
} // namespace bio

std::string to_string(bio::byte_string const& line)
{
	return {(char const*)line.data(), line.size()};
}

//using namespace std::literals;
TEST_CASE("constructors", "[]")
{
	SECTION("Container")
	{
		std::ifstream ifs("");

		bio::byte_string line;
		bio::byte_string lines;

		while(bio::getline(std::cin, line))
		{
			bug_var(to_string(line));
			lines = lines + line;
			bug_var(to_string(lines));
		}
	}
}
