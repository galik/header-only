#ifndef GALIK_STRING_UTILS_H_
#define GALIK_STRING_UTILS_H_
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

#include <algorithm>
#include <experimental/string_view>
#include "string_view_stream.h"

namespace galik {

using std::experimental::string_view;
using std::experimental::basic_string_view;

constexpr const char* const ws = " \t\n\r\f\v";

// mute

/**
 * Remove leading characters from a std::string.
 * @param s The std::string to be modified.
 * @param t The set of characters to delete from the beginning
 * of the string.
 * @return The same string passed in as a parameter reference.
 */
inline std::string& ltrim_mute(std::string& s, const char* t = ws)
{
	s.erase(0, s.find_first_not_of(t));
	return s;
}

/**
 * Remove trailing characters from a std::string.
 * @param s The std::string to be modified.
 * @param t The set of characters to delete from the end
 * of the string.
 * @return The same string passed in as a parameter reference.
 */
inline std::string& rtrim_mute(std::string& s, const char* t = ws)
{
	s.erase(s.find_last_not_of(t) + 1);
	return s;
}

/**
 * Remove surrounding characters from a std::string.
 * @param s The string to be modified.
 * @param t The set of characters to delete from each end
 * of the string.
 * @return The same string passed in as a parameter reference.
 */
inline std::string& trim_mute(std::string& s, const char* t = ws)
{
	return ltrim_mute(rtrim_mute(s, t), t);
}

// view

inline string_view ltrim_view(string_view s, const char* t = ws)
{
	auto pos = s.find_first_not_of(t);
	if(pos == string_view::npos)
		pos = s.size();
	s.remove_prefix(pos);
	return s;
}

inline string_view rtrim_view(string_view s, const char* t = ws)
{
	auto pos = s.find_last_not_of(t) + 1;
	if(!pos)
		pos = s.size();
	s.remove_suffix(s.size() - pos);
	return s;
}

inline string_view trim_view(string_view s, const char* t = ws)
{
	return ltrim_view(rtrim_view(s, t), t);
}

// copy

inline std::string ltrim_copy(std::string s, const char* t = ws)
{
	return ltrim_mute(s, t);
}

inline std::string rtrim_copy(std::string s, const char* t = ws)
{
	return rtrim_mute(s, t);
}

inline std::string trim_copy(std::string s, const char* t = ws)
{
	return trim_mute(s, t);
}

/**
 * Remove all leading characters of a given value
 * from a std::string.
 * @param s The string to be modified.
 * @param c The character value to delete.
 * @return The same string passed in as a parameter reference.
 */
inline std::string& ltrim_mute(std::string& s, char c)
{
	s.erase(0, s.find_first_not_of(c));
	return s;
}

/**
 * Remove all trailing characters of a given value
 * from a std::string.
 * @param s The string to be modified.
 * @param c The character value to delete.
 * @return The same string passed in as a parameter reference.
 */
inline std::string& rtrim_mute(std::string& s, char c)
{
	s.erase(s.find_last_not_of(c) + 1);
	return s;
}

/**
 * Remove all surrounding characters of a given value
 * from a std::string.
 * @param s The string to be modified.
 * @param c The character value to delete.
 * @return The same string passed in as a parameter reference.
 */
inline std::string& trim_mute(std::string& s, char c)
{
	return ltrim_mute(rtrim_mute(s, c), c);
}

inline std::string rtrim_copy(std::string s, char c)
{
	return rtrim_mute(s, c);
}

inline std::string ltrim_copy(std::string s, char c)
{
	return ltrim_mute(s, c);
}

inline std::string trim_copy(std::string s, char c)
{
	return trim_mute(s, c);
}

inline std::string ltrim_mute_keep(std::string& s, const char* t = ws)
{
	std::string::size_type pos;
	std::string keep = s.substr(0, (pos = s.find_first_not_of(t)));
	s.erase(0, pos);
	return keep;
}

inline std::string rtrim_mute_keep(std::string& s, const char* t = ws)
{
	std::string::size_type pos;
	std::string keep = s.substr((pos = s.find_last_not_of(t) + 1));
	s.erase(pos);
	return keep;
}

// upper lower

inline
std::string& lower_mute(std::string& s)
{
	std::transform(s.begin(), s.end(), s.begin()
		, std::ptr_fun<int, int>(std::tolower));
	return s;
}

inline
std::string& upper_mute(std::string& s)
{
	std::transform(s.begin(), s.end(), s.begin()
		, std::ptr_fun<int, int>(std::toupper));
	return s;
}

inline
std::string lower_copy(std::string s)
{
	return lower_mute(s);
}

inline
std::string upper_copy(std::string s)
{
	return upper_mute(s);
}

// split

/**
 * Split a StringType into a std::vector<StringType> at every character where
 * Comparator(Char) returns true.
 *
 * NOTE: Both the parameter StringType and the return StringType are the
 * same so if you want to return a std::vector<string_view>, for example, from a
 * a std::string you can do this:
 *
 * auto v = split_at_space(string_view(s), std::ptr_fun<int, int>(std::isspace));
 *
 * @param s A StringType to be split
 * @param fold If true then fold (ignore adjacent) delimiters
 * @return A std::vector<StringType> containing the split string
 */
template<typename StringType, typename Comparator>
std::vector<StringType> split(StringType&& s, Comparator cmp, bool fold = true)
{
	std::vector<StringType> v;

	auto done = s.data() + s.size();
	auto end = s.data();
	auto pos = end;

	if(fold)
	{
		while((pos = std::find_if_not(end, done, cmp)) != done)
		{
			end = std::find_if(pos, done, cmp);
			v.emplace_back(pos, end - pos);
		}
	}
	else
	{
		while((end = std::find_if(pos, done, cmp)) != done)
		{
			v.emplace_back(pos, end - pos);
			pos = end + 1;
		}

		if(pos != done)
			v.emplace_back(pos, end - pos);
	}

	return v;
}

/**
 * Split a StringType at every whitespace character into a std::vector<StringType>.
 * A whitespace character is determined by calling `std::isspace`.
 *
 * NOTE: Both the parameter StringType and the return StringType are the
 * same so if you want to return a std::vector<string_view>, for example, from a
 * a std::string you can do this:
 *
 * auto v = split_at_space(string_view(s));
 *
 * @param s A StringType to be split
 * @param fold If true then fold (ignore adjacent) delimiters
 * @return A std::vector<StringType> containing the split string
 */
template<typename StringType>
std::vector<StringType> split_at_space(StringType&& s, bool fold = true)
{
	return split(std::forward<StringType>(s), std::ptr_fun<int, int>(std::isspace), fold);
}

/**
 * Split a StringType at every character delim into a std::vector<StringType>
 *
 * NOTE: Both the parameter StringType and the return StringType are the
 * same so if you want to return a std::vector<string_view>, for example, from a
 * a std::string you can do this:
 *
 * auto v = split_at_delim(string_view(s), '\n');
 *
 * @param s A StringType to be split
 * @param delim A delimiting character
 * @param fold If true then fold (ignore adjacent) delimiters
 * @return A std::vector<StringType> containing the split string
 */
template<typename StringType>
std::vector<StringType> split_at_delim(StringType&& s
	, typename StringType::value_type delim, bool fold = true)
{
	using Char = typename StringType::value_type;
	return split(std::forward<StringType>(s), [delim](Char c){return c == delim;}, fold);
}

/**
 * Split a StringType at every string_view sequence delim into a std::vector<StringType>
 *
 * NOTE: Both the parameter StringType and the return StringType are the
 * same so if you want to return a std::vector<string_view>, for example, from a
 * a std::string you can do this:
 *
 * auto v = split_at_delim(string_view(s), "&&");
 *
 * @param s A StringType to be split
 * @param delim A delimiting string_view
 * @return A std::vector<StringType> containing the split string
 */
template<typename StringType>
std::vector<StringType> split_at_delim(StringType&& s
	, basic_string_view<typename StringType::value_type> delim)
{
	std::vector<StringType> v;

	auto done = s.data() + s.size();
	auto end = s.data();
	auto pos = end;

	while((end = std::search(pos, done, delim.begin(), delim.end())) != done)
	{
		v.emplace_back(pos, end - pos);
		pos = end + delim.size();
	}

	if(pos != done)
		v.emplace_back(pos, end - pos);

	return v;
}

/**
 * Split a StringType at every delimiter provided in delims into a std::vector<StringType>
 *
 * NOTE: Both the parameter StringType and the return StringType are the
 * same so if you want to return a std::vector<string_view>, for example, from a
 * a std::string you can do this:
 *
 * auto v = split_at_delims(string_view(s), delims);
 *
 * @param s A StringType to be split
 * @param delims A std::experimental::string_view of delimiting characters
 * @param fold If true then fold (ignore adjacent) delimiters
 * @return A std::vector<StringType> containing the split string
 */
template<typename StringType>
std::vector<StringType> split_at_delims(StringType&& s
	, basic_string_view<typename StringType::value_type> delims, bool fold = true)
{
	using Char = typename StringType::value_type;
	return split(std::forward<StringType>(s), [delims](Char c){return delims.find(c) != StringType::npos;}, fold);
}

} // galik

#endif // GALIK_STRING_UTILS_H_
