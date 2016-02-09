////
//// Copyright (c) 2016 Galik <galik.bool@gmail.com>
////
//// Permission is hereby granted, free of charge, to any person obtaining a copy of
//// this software and associated documentation files (the "Software"), to deal in
//// the Software without restriction, including without limitation the rights to
//// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
//// of the Software, and to permit persons to whom the Software is furnished to do
//// so, subject to the following conditions:
////
//// The above copyright notice and this permission notice shall be included in all
//// copies or substantial portions of the Software.
////
//// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//// SOFTWARE.
////
//
//#include <bitset>
//#include <iomanip>
//#include <iostream>
//
//#include <fstream>
//
//#include "test.h"
//#include "u8string.h"
//#include "string_utils.h"
//
//using namespace hol;
//using namespace std::literals::string_literals;
//
//const u8string test_data[] =
//{
//	  u8"κόσμε"s
//	, u8""s
//};
//
//int main()
//{
////	cout << u8"\x61\xE0\xA4\xA8\xE0\xA4\xBF\xE4\xBA\x9C\xF0\x90\x82\x83" << endl;
////	cout << u8"\u0061\u0928\u093F\u4E9C\u10083" << endl;
//
//	u8string u8s = "\x61\xE0\xA4\xA8\xE0\xA4\xBF\xE4\xBA\x9C\xF0\x90\x82\x83";
//
//	if(u8string::report r = u8s.valid())
//		con("u8s: " << u8s.string());
//
//
////
////	for(auto&& test: test_data)
////	{
////		con("test : " << test.string());
////		r = test.valid();
////		con("valid: " << r);
////		if(!r)
////		{
////			con('\t' << test[0].size());
////			con('\t' << r.msg + " at: " + std::to_string(r.pos));
////		}
////	}
////
////	u8string u8s = u8" x ";
////
////	ltrim_mute(u8s, u8string(" "));
//}
//
//
//
//
//
//
int main() {}
