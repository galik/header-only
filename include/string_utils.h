#ifndef HOL_STRING_UTILS_H
#define HOL_STRING_UTILS_H
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
#include <algorithm>
#include <experimental/string_view>

namespace hol {

using std::string;
using std::vector;
using std::experimental::string_view;

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

// Template version

template<typename StringType>
inline StringType& ltrim_mute(StringType& s, const StringType& t = {})
{
//	s.erase(0, s.find_first_not_of(t));
	return s;
}

//inline std::string& rtrim_mute(std::string& s, const char* t = ws)
//{
//	s.erase(s.find_last_not_of(t) + 1);
//	return s;
//}
//
//inline std::string& trim_mute(std::string& s, const char* t = ws)
//{
//	return ltrim_mute(rtrim_mute(s, t), t);
//}


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

template<typename IterType, typename ComparatorType, typename ContainerType>
void split(IterType pos, IterType done, ComparatorType cmp, ContainerType& v, bool fold = true)
{
	auto end = pos;

	if(fold)
	{
		while((pos = std::find_if_not(end, done, cmp)) != done)
		{
			end = std::find_if(pos, done, cmp);
			if(end > pos)
				v.emplace_back(pos, end - pos);
		}
	}
	else
	{
		while((end = std::find_if(pos, done, cmp)) != done)
		{
			if(end > pos)
				v.emplace_back(pos, end - pos);
			pos = end + 1;
		}

		if(pos != done)
			v.emplace_back(pos, end - pos);
	}
}

template<typename ComparatorType, typename ContainerType>
void split(string_view s, ComparatorType cmp, ContainerType& v, bool fold = true)
{
	split(s.data(), s.data() + s.size(), cmp, v, fold);
}

template<typename ComparatorType>
vector<string> split_copy(string_view s, ComparatorType cmp, bool fold = true)
{
	vector<string> v;
	split(s, cmp, v, fold);
	return v;
}

template<typename ComparatorType>
vector<string_view> split_view(string_view s, ComparatorType cmp, bool fold = true)
{
	vector<string_view> v;
	split(s, cmp, v, fold);
	return v;
}

inline
vector<string> split_copy_at_space(string_view s, bool fold = true)
{
	return split_copy(s, std::ptr_fun<int, int>(std::isspace), fold);
}

inline
vector<string_view> split_view_at_space(string_view s, bool fold = true)
{
	return split_view(s, std::ptr_fun<int, int>(std::isspace), fold);
}

inline
vector<string> split_copy_at_delim(string_view s, char delim, bool fold = true)
{
	return split_copy(s, [delim](char c){return c == delim;}, fold);
}

inline
vector<string_view> split_view_at_delim(string_view s, char delim, bool fold = true)
{
	return split_view(s, [delim](char c){return c == delim;}, fold);
}

inline
vector<string> split_copy_at_delims(string_view s, string_view delims, bool fold = true)
{
	return split_copy(s, [delims](char c){return delims.find(c) != string_view::npos;}, fold);
}

inline
vector<string_view> split_view_at_delims(string_view s, string_view delims, bool fold = true)
{
	return split_view(s, [delims](char c){return delims.find(c) != string_view::npos;}, fold);
}

template<typename ContainerType>
void split_at_delim(string_view s, string_view delim, ContainerType& v)
{
	auto done = s.data() + s.size();
	auto end = s.data();
	auto pos = end;

	while((end = std::search(pos, done, delim.begin(), delim.end())) != done)
	{
		if(end > pos)
			v.emplace_back(pos, end - pos);
		pos = end + delim.size();
	}

	if(pos != done)
		if(end > pos)
			v.emplace_back(pos, end - pos);
}

inline
vector<string> split_copy_at_delim(string_view s, string_view delim)
{
	vector<string> v;
	split_at_delim(s, delim, v);
	return v;
}

inline
vector<string_view> split_view_at_delim(string_view s, string_view delim)
{
	vector<string_view> v;
	split_at_delim(s, delim, v);
	return v;
}

template<typename ContainerType>
void split_at_regex(std::regex e, string_view s, ContainerType& v)
{
	auto pos = s.data();
	auto end = pos + s.size();
	std::cmatch m;

	while(std::regex_search(pos, end, m, e))
	{
		if(!m.empty())
			v.emplace_back(pos, m.position());
		pos = pos + m.position() + m.length();
	}

	if(end > pos)
		v.emplace_back(pos, end - pos);
}

inline
vector<string> split_copy_at_regex(std::regex e, string_view s, std::size_t reserve_guess = 20)
{
	vector<string> v;
	v.reserve(reserve_guess);
	split_at_regex(e, s, v);
	return v;
}

inline
vector<string_view> split_view_at_regex(std::regex e, string_view s, std::size_t reserve_guess = 20)
{
	vector<string_view> v;
	v.reserve(reserve_guess);
	split_at_regex(e, s, v);
	return v;
}

inline
vector<string> split_copy_at_regex(string const& e, string_view s, std::size_t reserve_guess = 20)
{
	vector<string> v;
	v.reserve(reserve_guess);
	split_at_regex(std::regex(e), s, v);
	return v;
}

inline
vector<string_view> split_view_at_regex(string const& e, string_view s, std::size_t reserve_guess = 20)
{
	vector<string_view> v;
	v.reserve(reserve_guess);
	split_at_regex(std::regex(e), s, v);
	return v;
}

} // hol

#endif // HOL_STRING_UTILS_H
