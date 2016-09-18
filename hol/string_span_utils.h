#ifndef HOL_STRING_SPAN_UTILS_H
#define HOL_STRING_SPAN_UTILS_H
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
#include <algorithm>

#include <gsl/gsl_assert>
#include <gsl/string_span>

namespace hol {

using offset_type = std::ptrdiff_t;

template<typename CharT>
gsl::basic_string_span<CharT> substr(
	gsl::basic_string_span<CharT> s,
	offset_type offset,
	offset_type count)
{
	Expects(offset >= 0);
	Expects(offset <= s.size());
	return s.subspan(offset, count);
}

template<typename Char1, typename Char2>
offset_type find(
	gsl::basic_string_span<Char1> s,
	Char2 c,
	offset_type offset = 0)
{
	Expects(offset >= 0);
	Expects(offset <= s.size());
	for(; offset < s.size(); ++offset)
		if(s[offset] == c)
			return offset;
	return s.size();
}

template<typename Char1, typename Char2>
offset_type find_first_of(
	gsl::basic_string_span<Char1> s,
	gsl::basic_string_span<Char2> r,
	offset_type offset = 0)
{
	Expects(offset >= 0);
	Expects(offset <= s.size());
	for(; offset < s.size(); ++offset)
		if(find(r, s[offset]) != r.size())
			return offset;
	return s.size();
}

template<typename Char1, typename Char2>
offset_type find_first_not_of(
	gsl::basic_string_span<Char1> s,
	gsl::basic_string_span<Char2> r,
	offset_type offset = 0)
{
	Expects(offset >= 0);
	Expects(offset <= s.size());
	for(; offset < s.size(); ++offset)
		if(find(r, s[offset]) == r.size())
			return offset;
	return s.size();
}

template<typename Char1, typename Char2>
offset_type find_last_of(
	gsl::basic_string_span<Char1> s,
	gsl::basic_string_span<Char2> r,
	offset_type offset = -1)
{
	Expects(offset >= -1);
	Expects(offset <= s.size());

	if(s.empty())
		return 0;

	for(offset = offset < 0 ? s.size():offset + 1; offset; --offset)
		if(find(r, s[offset - 1]) != r.size())
			return offset - 1;
	return s.size();
}

template<typename Char1, typename Char2>
offset_type find_last_not_of(
	gsl::basic_string_span<Char1> s,
	gsl::basic_string_span<Char2> r,
	offset_type offset = -1)
{
	Expects(offset >= -1);
	Expects(offset <= s.size());

	if(s.empty())
		return 0;

	for(offset = offset < 0 ? s.size():offset + 1; offset; --offset)
		if(find(r, s[offset - 1]) == r.size())
			return offset - 1;
	return s.size();
}

} // hol

#endif // HOL_STRING_SPAN_UTILS_H
