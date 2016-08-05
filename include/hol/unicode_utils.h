#ifndef HOL_UNICODE_UTILS_H
#define HOL_UNICODE_UTILS_H
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

// TODO: remove this when condition is
// fully applied to all uses
//#define HOL_USE_STRING_VIEW

#include <string>
#include <locale>
#include <codecvt>

namespace hol {

//std::string to_utf8(std::wstring w)
//{
//    return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(w);
//}
//
//std::wstring from_utf8(std::string s)
//{
//    return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().from_bytes(s);
//}

using wcodecvt_utf8 = std::codecvt_utf8<wchar_t>;
using ucs2codecvt_utf8 = std::codecvt_utf8<char16_t>;
using ucs4codecvt_utf8 = std::codecvt_utf8<char32_t>;

using wstring_convert_utf8 = std::wstring_convert<wcodecvt_utf8, wchar_t>;
using ucs2string_convert_utf8 = std::wstring_convert<ucs2codecvt_utf8, char16_t>;
using ucs4string_convert_utf8 = std::wstring_convert<ucs4codecvt_utf8, char32_t>;

template<typename StringType>
struct utf8_converter_type
{
	using type = typename std::conditional
	<
		std::is_same<StringType, std::wstring>::value,
		wstring_convert_utf8,
		typename std::conditional
		<
			std::is_same<StringType, std::u16string>::value,
			ucs2string_convert_utf8,
			ucs4string_convert_utf8
		>::type
	>::type;
};

template<typename StringType>
std::string ucs_to_utf8(StringType const& s)
{
	typename utf8_converter_type<StringType>::type cnv;
	std::string utf8 = cnv.to_bytes(s);
	if(cnv.converted() < s.size())
		throw std::runtime_error("incomplete conversion");
	return utf8;
}

template<typename StringType>
StringType utf8_to_ucs(std::string const& utf8)
{
	typename utf8_converter_type<StringType>::type cnv;
	StringType s = cnv.from_bytes(utf8);
	if(cnv.converted() < utf8.size())
		throw std::runtime_error("incomplete conversion");
	return s;
}

inline
std::string ucs2_to_utf8(std::u16string const& ucs2)
{
	return ucs_to_utf8(ucs2);
}

inline
std::string ucs4_to_utf8(std::u32string const& ucs4)
{
	return ucs_to_utf8(ucs4);
}

inline
std::string w_to_utf8(std::wstring const& ucs)
{
	return ucs_to_utf8(ucs);
}

inline
std::u16string utf8_to_ucs2(std::string const& utf8)
{
	return utf8_to_ucs<std::u16string>(utf8);
}

inline
std::u32string utf8_to_ucs4(std::string const& utf8)
{
	return utf8_to_ucs<std::u32string>(utf8);
}

inline
std::wstring utf8_to_w(std::string const& utf8)
{
	return utf8_to_ucs<std::wstring>(utf8);
}

//std::wstring utf8_to_ws(std::string const& utf8)
//{
//	wstring_convert_utf8 cnv;
//	std::wstring w = cnv.from_bytes(utf8);
//	if(cnv.converted() < utf8.size())
//		throw std::runtime_error("incomplete conversion");
//	return w;
//}
//
//std::string w_to_utf8(std::wstring const& w)
//{
//	wstring_convert_utf8 cnv;
//	std::string utf8 = cnv.to_bytes(w);
//	if(cnv.converted() < w.size())
//		throw std::runtime_error("incomplete conversion");
//	return utf8;
//}

} // hol

#endif // HOL_UNICODE_UTILS_H
