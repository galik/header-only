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

#include <bitset>
#include <iomanip>
#include <iostream>

#include <fstream>

#include "test.h"
#include "u8string.h"
#include "string_utils.h"

using namespace hol;
using namespace std::literals::string_literals;

const u8string test_data[] =
{
	  u8"κόσμε"s
	, u8""s
};

int main()
{
//	std::string sep = "  ";
//	std::ofstream ofs("u8char-size.cpp");
//	ofs << "constexpr static const std::vector<char> u8char_size =\n{";
	for(int i = 0; i < 256; ++i)
	{
//		if(!(i % 16))
//			ofs << '\n' << '\t';
//		ofs << sep << int(get_lookup()[i]);
//		sep = ", ";
		if(u8char(char(i)).size() != get_lookup()[i])
		{
			con("bad: " << std::bitset<8>(i));
			bug_var(u8char(char(i)).size());
			bug_var(get_lookup()[i]);
		}
	}
//	ofs << "\n};\n";

	return 0;

	u8string::report r;

	for(auto&& test: test_data)
	{
		con("test : " << test.string());
		r = test.valid();
		con("valid: " << r);
		if(!r)
		{
			con('\t' << test[0].size());
			con('\t' << r.msg + " at: " + std::to_string(r.pos));
		}
	}

	u8string u8s = u8" x ";

	ltrim_mute(u8s, u8string(" "));
}






