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
#include "hol/u8string.h"
#include "hol/string_utils.h"

using namespace hol;
using namespace std::literals::string_literals;

const u8string test_data[] =
{
	  u8"κόσμε"s
	, u8""s
};

const std::string test_data2[] =
{
	  "\u00000000"
	, "\u00000080"
	, "\u00000800"
	, "\u00010000"
	, "\u00200000"
	, "\u04000000"
};

int main()
{
//	OUT(u8"\x61\xE0\xA4\xA8\xE0\xA4\xBF\xE4\xBA\x9C\xF0\x90\x82\x83");
//	OUT(u8"\u0061\u0928\u093F\u4E9C\u10083");
//
//	u8string u8s = u8"\u0061\u0928\u093F\u4E9C\u10083";//"\x61\xE0\xA4\xA8\xE0\xA4\xBF\xE4\xBA\x9C\xF0\x90\x82\x83";
//
//	if(u8string::report r = u8s.valid())
//		OUT("u8s: " << u8s.string());

	for(auto&& test: test_data)
	{
		OUT("test : (" << test.string().size() << ") " << test.string());
		auto r = test.valid();
		OUT("valid: " << r);
		if(!r)
		{
			OUT('\t' << test[0].size());
			OUT('\t' << r.msg + " at: " + std::to_string(r.pos));
		}
	}

	for(auto&& test: test_data2)
	{
//		OUT("test : (" << test.string().size() << ") " << test.string());
		OUT("test : (" << test.size() << ") " << test);
//		auto r = test.valid();
//		OUT("valid: " << r);
//		if(!r)
//		{
//			OUT('\t' << test[0].size());
//			OUT('\t' << r.msg + " at: " + std::to_string(r.pos));
//		}
	}

//
//	u8string u8s = u8" x ";
//
//	ltrim_mute(u8s, u8string(" "));
}






