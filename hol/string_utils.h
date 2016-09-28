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
#include <cerrno>
#include <string>
#include <vector>
#include <locale>
#include <cstdlib> // std::strtol
#include <codecvt>
#include <cstring>
#include <fstream>
#include <sstream>
#include <algorithm>

#ifdef HOL_USE_STRING_VIEW
#	include <experimental/string_view>
#endif

#ifdef HOL_USE_STRING_SPAN
#	include <gsl/string_span>
#endif

namespace hol {

#ifdef HOL_USE_STRING_VIEW
using string_view = std::experimental::string_view;
#endif

#ifdef HOL_USE_STRING_SPAN
using string_span = gsl::string_span<>;
#endif

// replace_all

//inline
//std::string& replace_all(std::string& s, const std::string& from, const std::string& to)
//{
//	if(!from.empty())
//		for(size_t pos = 0; (pos = s.find(from, pos)) != std::string::npos; pos += to.size())
//			s.replace(pos, from.size(), to);
//	return s;
//}

template<typename CharT>
std::basic_string<CharT>& replace_all_mute(
	std::basic_string<CharT>& s,
	const std::basic_string<CharT>& from,
	const std::basic_string<CharT>& to)
{
	if(!from.empty())
		for(std::size_t pos = 0; (pos = s.find(from, pos) + 1); pos += to.size())
			s.replace(--pos, from.size(), to);
	return s;
}

inline
std::string& replace_all_mute(std::string& s,
	const std::string& from, const std::string& to)
{
	return replace_all_mute<char>(s, from, to);
}

inline
std::wstring& replace_all_mute(std::wstring& s,
	const std::wstring& from, const std::wstring& to)
{
	return replace_all_mute<wchar_t>(s, from, to);
}

inline
std::string replace_all_copy(std::string s,
	const std::string& from, const std::string& to)
{
	return replace_all_mute(s, from, to);
}

inline
std::wstring replace_all_copy(std::wstring s,
	const std::wstring& from, const std::wstring& to)
{
	return replace_all_mute(s, from, to);
}

// upper and lower

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

namespace detail {
constexpr char const* empty(char) { return ""; }
constexpr wchar_t const* empty(wchar_t) { return L""; }
constexpr char const* space(char) { return " "; }
constexpr wchar_t const* space(wchar_t) { return L" "; }
} // detail

/**
 * Usage:
 *
 * output_separator sep;
 *
 * for(auto const& s: v)
 *     std::cout << sep << s;
 * std::cout << '\n';
 *
 */
template<typename CharT>
class basic_output_separator
{
	CharT const* init;
	CharT const* s;
	CharT const* next;

public:
	basic_output_separator(): init(detail::empty(CharT())), s(init), next(detail::space(CharT())) {}
	basic_output_separator(CharT const* next): init(detail::empty(CharT())), s(init), next(next) {}
	basic_output_separator(CharT const* init, CharT const* next): init(init), s(init), next(next) {}
	basic_output_separator(basic_output_separator const& sep): s(sep.s), next(sep.next) {}

	template<typename CharU>
	friend std::basic_ostream<CharU>& operator<<(std::basic_ostream<CharU>& os,
		basic_output_separator<CharU>& sep)
	{
		os << sep.s;
		sep.s = sep.next;
		return os;
	}

	/**
	 * The first time this is called after construction or calling reset()
	 * this function appends the output_separator's initial state to the string,
	 * thereafter the second state is appended. By  default the initial state is
	 * empty and the second state is a single space.
	 *
	 * @param s The std::string to be appended to
	 * @param sep The output_separator object to be appended to the string.
	 * @return A new std::string with the first or second state of the
	 * output_separator appended to the input std::string
	 */
	template<typename CharU>
	friend std::basic_string<CharU> operator+(std::basic_string<CharU> const& s,
		basic_output_separator<CharU>& sep)
	{
		std::basic_string<CharU> r = s + sep.s;
		sep.s = sep.next;
		return r;
	}

	/**
	 * The first time this is called after construction or calling reset()
	 * this function prepends the output_separator's initial state to the string,
	 * thereafter the second state is prepended. By  default the initial state is
	 * empty and the second state is a single space.
	 *
	 * @param s The std::string to be prepended to
	 * @param sep The output_separator object to be prepended to the string.
	 * @return A new std::string with the first or second state of the
	 * output_separator prepended to the input std::string
	 */
	template<typename CharU>
	friend std::basic_string<CharU> operator+(basic_output_separator<CharU>& sep,
		std::basic_string<CharU> const& s)
	{
		std::basic_string<CharU> r = sep.s + s;
		sep.s = sep.next;
		return r;
	}

	/**
	 * The first time this is called after construction or calling reset()
	 * this function appends the output_separator's initial state to the string,
	 * thereafter the second state is appended. By  default the initial state is
	 * empty and the second state is a single space.
	 *
	 * @param s The std::string to be appended to
	 * @param sep The output_separator object to be appended to the string.
	 * @return A new std::string with the first or second state of the
	 * output_separator appended to the input std::string
	 */
	template<typename CharU>
	friend std::basic_string<CharU> operator+(CharU const* s,
		basic_output_separator<CharU>& sep)
	{
		std::basic_string<CharT> r{s};
		r += sep.s;
		sep.s = sep.next;
		return r;
	}

	/**
	 * The first time this is called after construction or calling reset()
	 * this function prepends the output_separator's initial state to the string,
	 * thereafter the second state is prepended. By  default the initial state is
	 * empty and the second state is a single space.
	 *
	 * @param s The std::string to be prepended to
	 * @param sep The output_separator object to be prepended to the string.
	 * @return A new std::string with the first or second state of the
	 * output_separator prepended to the input std::string
	 */
	template<typename CharU>
	friend std::basic_string<CharU> operator+(basic_output_separator<CharU>& sep,
		CharU const* s)
	{
		std::basic_string<CharU> r{sep.s};
		r += s;
		sep.s = sep.next;
		return r;
	}

	/**
	 * Reset the optput_separator to its initial state so that
	 * the next output function will receive the initial state and
	 * thereafter the second state.
	 */
	void reset() { this->s = init; }

	/**
	 * Reset the optput_separator to a new initial state `init` so that
	 * the next output function will receive the new initial state and
	 * thereafter the previously configured second state.
	 */
	void reset(CharT const* init) { this->s = this->init = init; }

	/**
	 * Reset the optput_separator to a new initial state `init` and
	 * with a new second state `ext` so that the next output function
	 * will receive the new initial state and
	 * thereafter the newly configured second state.
	 */
	void reset(CharT const* init, CharT const* next)
	{
		this->s = this->init = init;
		this->next = next;
	}
};

using output_separator = basic_output_separator<char>;
using woutput_separator = basic_output_separator<wchar_t>;

// trimming functions

constexpr const char ws[] = " \t\n\r\f\v\0";

/**
 * Remove leading characters from a str.
 * @param s The std::string to be modified.
 * @param t The set of characters to delete from the beginning
 * of the string.
 * @return The same string passed in as a parameter reference.
 */
inline
std::string& trim_left_mute(std::string& s, const char* t = ws)
{
	s.erase(0, s.find_first_not_of(t));
	return s;
}

/**
 * Remove trailing characters from a str.
 * @param s The std::string to be modified.
 * @param t The set of characters to delete from the end
 * of the string.
 * @return The same string passed in as a parameter reference.
 */
inline
std::string& trim_right_mute(std::string& s, const char* t = ws)
{
	s.erase(s.find_last_not_of(t) + 1);
	return s;
}

/**
 * Remove surrounding characters from a str.
 * @param s The string to be modified.
 * @param t The set of characters to delete from each end
 * of the string.
 * @return The same string passed in as a parameter reference.
 */
inline
std::string& trim_mute(std::string& s, const char* t = ws)
{
	return trim_left_mute(trim_right_mute(s, t), t);
}

// MOVE SEMANTICS

/**
 * Remove leading characters from a str.
 * @param s The std::string to be modified.
 * @param t The set of characters to delete from the beginning
 * of the string.
 * @return The same string passed in as a parameter reference.
 */
inline
std::string trim_left_mute(std::string&& s, const char* t = ws)
{
	return trim_left_mute(s, t);
}

/**
 * Remove trailing characters from a str.
 * @param s The std::string to be modified.
 * @param t The set of characters to delete from the end
 * of the string.
 * @return The same string passed in as a parameter reference.
 */
inline
std::string trim_right_mute(std::string&& s, const char* t = ws)
{
	return trim_right_mute(s, t);
}

/**
 * Remove surrounding characters from a str.
 * @param s The string to be modified.
 * @param t The set of characters to delete from each end
 * of the string.
 * @return The same string passed in as a parameter reference.
 */
inline
std::string trim_mute(std::string&& s, const char* t = ws)
{
	return trim_left_mute(trim_right_mute(s, t), t);
}

inline
std::string trim_left_copy(std::string s, const char* t = ws)
{
	return trim_left_mute(s, t);
}

inline
std::string trim_right_copy(std::string s, const char* t = ws)
{
	return trim_right_mute(s, t);
}

inline
std::string trim_copy(std::string s, const char* t = ws)
{
	return trim_mute(s, t);
}

// keep

inline
std::string trim_left_keep(std::string& s, const char* t = ws)
{
	std::string::size_type pos;
	std::string keep = s.substr(0, (pos = s.find_first_not_of(t)));
	s.erase(0, pos);
	return keep;
}

inline
std::string trim_right_keep(std::string& s, const char* t = ws)
{
	std::string::size_type pos;
	std::string keep = s.substr((pos = s.find_last_not_of(t) + 1));
	s.erase(pos);
	return keep;
}

inline
auto trim_keep(std::string& s, const char* t = ws)
{
	struct rv
	{
		std::string left;
		std::string right;
	};

	return rv{trim_left_keep(s, t), trim_right_keep(s, t)};
}

#ifdef HOL_USE_STRING_VIEW
//views
inline
string_view trim_left_view(string_view s, const char* t = ws)
{
	if(auto pos = s.find_first_not_of(t) + 1)
	{
		s.remove_prefix(pos - 1);
		return {s.data(), s.size()};
	}
	return {};
}

inline
string_view trim_right_view(string_view s, const char* t = ws)
{
	if(auto pos = s.find_last_not_of(t) + 1)
		s.remove_suffix(s.size() - pos);
	return {s.data(), s.size()};
}

inline
string_view trim_view(string_view s, const char* t = ws)
{
	return trim_left_view(trim_right_view(s, t), t);
}

#endif

inline std::string& trim_left_mute(std::string& s, char c)
{
	s.erase(0, s.find_first_not_of(c));
	return s;
}

inline std::string& trim_right_mute(std::string& s, char c)
{
	s.erase(s.find_last_not_of(c) + 1);
	return s;
}

inline std::string& trim_mute(std::string& s, char c)
{
	return trim_left_mute(trim_right_mute(s, c), c);
}

inline std::string& trim_left_mute(std::string&& s, char c)
{
	return trim_left_mute(s, c);
}

inline std::string& trim_right_mute(std::string&& s, char c)
{
	return trim_right_mute(s, c);
}

inline std::string& trim_mute(std::string&& s, char c)
{
	return trim_left_mute(trim_right_mute(s, c), c);
}

inline std::string trim_right_copy(std::string s, char c)
{
	return trim_right_mute(s, c);
}

inline std::string trim_left_copy(std::string s, char c)
{
	return trim_left_mute(s, c);
}

inline std::string trim_copy(std::string s, char c)
{
	return trim_mute(s, c);
}

// GSL_STRING_SPAN

#ifdef HOL_USE_STRING_SPAN
namespace gsl_detail {
gsl::cstring_span<> ws{::hol::ws};

inline
string_span::index_type find_first_not_of(string_span s, gsl::cstring_span<> t)
{
	for(string_span::index_type i = 0; i < s.size(); ++i)
		if(std::find(t.begin(), t.end(), s[i]) == t.end())
			return i;
	return -1; // 0?
}

inline
string_span::index_type find_last_not_of(string_span s, gsl::cstring_span<> t)
{
	for(string_span::index_type i = s.size(); i; --i)
		if(std::find(t.begin(), t.end(), s[i - 1]) == t.end())
			return i - 1;
	return -1; // s.size() ?
}
} // gsl_detail

inline
string_span trim_left_span(string_span s, gsl::cstring_span<> t = gsl_detail::ws)
{
	auto found = gsl_detail::find_first_not_of(s, t);

	if(found == -1)
		return {};

	return s.subspan(found);
}

inline
string_span trim_right_span(string_span s, gsl::cstring_span<> t = gsl_detail::ws)
{
	return s.subspan(0, gsl_detail::find_last_not_of(s, t) + 1);
}

inline
string_span trim_span(string_span s, gsl::cstring_span<> t = gsl_detail::ws)
{
	return trim_left_span(trim_right_span(s, t), t);
}

#endif

// NEW DEFINITIVE SPLIT ALGORITHM??

// TODO: add char delim versions
// TODO: add view versions
// TODO: add span versions
// TODO: add multi_span versions

// split_with

template
<
	typename StringType = std::string
	, typename StringType2 = std::string
	, typename VectorType = std::vector<StringType>
>
auto basic_split(
	StringType const& s
	, VectorType& v
	, StringType2 const& delim_in
	, bool fold = true
	, bool strict = false) -> VectorType
{
	if(s.empty())
		return {};

	StringType delim = delim_in;

//	VectorType v;
//	v.reserve(reserve);

	if(delim.empty())
	{
		for(auto c: s)
			v.emplace_back(1, c);
		return v;
	}

	auto done = s.data() + s.size();
	auto end = s.data();
	auto pos = end;

	if((end = std::search(pos, done, delim.begin(), delim.end())) == done)
		return strict ? VectorType{}:VectorType{s};

	if(end > pos)
		v.emplace_back(pos, end - pos);
	else if(!fold)
		v.emplace_back();

	pos = end + delim.size();

	while((end = std::search(pos, done, delim.begin(), delim.end())) != done)
	{
		if(end > pos)
			v.emplace_back(pos, end - pos);
		else if(!fold)
			v.emplace_back();

		pos = end + delim.size();
	}

	if(end > pos)
		v.emplace_back(pos, end - pos);
	else if(!fold)
		v.emplace_back();

	return v;
}

template
<
	typename StringType = std::string
	, typename StringType2 = std::string
	, typename VectorType = std::vector<StringType>
>
auto basic_split(
	StringType const& s
	, StringType2 const& delim_in
	, bool fold = true
	, bool strict = false) -> VectorType
{
	VectorType v;
	return basic_split(s, v, delim_in, fold, strict);
}

/**
 *
 * @param s The string that is to be divided into pieces surrounding
 * the occurrence of specified a delimiter.
 * @param delim Delimiting string used to divide a larger string into pieces.
 * @param fold If true, ignores adjacent duplicate delimiters. So multiple spaces
 * are treated as one space (for example).
 * @param strict If true returns empty vector if delim not found (no splits)
 * otherwise returns whole string if delim not found.
 * @return A std::vector<std::string> containing all the pieces.
 */
inline
std::vector<std::string> split_copy(
	std::string const& s
	, std::string const& delim = " "
	, bool fold = true
	, bool strict = false)
{
	return basic_split(s, delim, fold, strict);
}

inline
std::vector<std::string> split_copy(
	std::string const& s
	, std::vector<std::string>& v
	, std::string const& delim = " "
	, bool fold = true
	, bool strict = false)
{
	return basic_split(s, v, delim, fold, strict);
}

inline
std::vector<std::wstring> split_copy(
	std::wstring const& s
	, std::wstring const& delim = L" "
	, bool fold = true
	, bool strict = false)
{
	return basic_split(s, delim, fold, strict);
}

inline
std::vector<std::wstring> split_copy(
	std::wstring const& s
	, std::vector<std::wstring>& v
	, std::wstring const& delim = L" "
	, bool fold = true
	, bool strict = false)
{
	return basic_split(s, v, delim, fold, strict);
}

// split_from (split string from selection of characters)
// grossly untested code, probably broken

template
<
	typename StringType = std::string
	, typename StringType2 = std::string
	, typename VectorType = std::vector<StringType>
>
auto basic_split_from(
	StringType const& s
	, VectorType& v
	, StringType2 const& delims_in
	, bool fold = true
	, bool strict = false) -> VectorType
{
	if(s.empty())
		return {};

	StringType delims = delims_in;

//	VectorType v;
//	v.reserve(reserve);

	if(delims.empty())
	{
		for(auto c: s)
			v.emplace_back(1, c);
		return v;
	}

	auto done = s.data() + s.size();
	auto end = s.data();
	auto pos = end;

	if((end = std::find_first_of(pos, done, delims.begin(), delims.end())) == done)
		return strict ? VectorType{}:VectorType{s};

	if(end > pos)
		v.emplace_back(pos, end - pos);
	else if(!fold)
		v.emplace_back();

	pos = std::find_first_of(end + 1, done, delims.begin(), delims.end(), [](auto c, auto d)
	{
		return c != d;
	});//end + 1;//delims.size();

	while((end = std::find_first_of(pos, done, delims.begin(), delims.end())) != done)
	{
		if(end > pos)
			v.emplace_back(pos, end - pos);
		else if(!fold)
			v.emplace_back();

		pos = std::find_first_of(end + 1, done, delims.begin(), delims.end(), [](auto c, auto d)
		{
			return c != d;
		});//end + 1;//delims.size();
	}

	if(end > pos)
		v.emplace_back(pos, end - pos);
	else if(!fold)
		v.emplace_back();

	return v;
}

template
<
	typename StringType = std::string
	, typename StringType2 = std::string
	, typename VectorType = std::vector<StringType>
>
auto basic_split_from(
	StringType const& s
	, StringType2 const& delims_in
	, bool fold = true
	, bool strict = false) -> VectorType
{
	VectorType v;
	return basic_split_from(s, v, delims_in, fold, strict);
}

/**
 *
 * @param s The string that is to be divided into pieces surrounding
 * the occurrence of specified set of delimiters.
 * @param delims String of delimiting characters used to divide a string into pieces.
 * @param fold If true, ignores adjacent duplicate delimiters. So multiple spaces
 * are treated as one space (for example).
 * @param strict If true returns empty vector if delim not found (no splits)
 * otherwise returns whole string if delim not found.
 * @return A std::vector<std::string> containing all the pieces.
 */
inline
std::vector<std::string> split_copy_from(
	std::string const& s
	, std::string const& delims = " "
	, bool fold = true
	, bool strict = false)
{
	return basic_split_from(s, delims, fold, strict);
}

inline
std::vector<std::string> split_copy_from(
	std::string const& s
	, std::vector<std::string>& v
	, std::string const& delims = " "
	, bool fold = true
	, bool strict = false)
{
	return basic_split_from(s, v, delims, fold, strict);
}

inline
std::vector<std::wstring> split_copy_from(
	std::wstring const& s
	, std::wstring const& delims = L" "
	, bool fold = true
	, bool strict = false)
{
	return basic_split_from(s, delims, fold, strict);
}

inline
std::vector<std::wstring> split_copy_from(
	std::wstring const& s
	, std::vector<std::wstring>& v
	, std::wstring const& delims = L" "
	, bool fold = true
	, bool strict = false)
{
	return basic_split_from(s, v, delims, fold, strict);
}

// JOIN

template<typename Container>
std::string join(const Container& c, std::string const& delim = " ")
{
	std::string ret, sep;
	for(auto const& s: c)
		{ ret += sep + s; sep = delim; }
	return ret;
}

// --------------------------------------------------------------------
// string conversions
// --------------------------------------------------------------------

// All these conversions return true only if a valid
// number surrounded only by spaces is found

// they accept the same input as their corresponding
// std::strtoxx() functions.

//bool s_to_l(const std::string& s, long& l);
//bool s_to_ll(const std::string& s, long long& ll);
//
//bool s_to_ul(const std::string& s, unsigned long& ul);
//bool s_to_ull(const std::string& s, unsigned long long& ull);
//
//bool s_to_f(const std::string& s, float& f);
//bool s_to_d(const std::string& s, double& d);
//bool s_to_ld(const std::string& s, long double& ld);

// string conversions

//inline
//bool s_to_test(char const* s, char const* e)
//{
//	if(e == s)
//		return false;
//
//	while(std::isspace(*e))
//		++e;
//
//	return !(*e);
//}
//
//inline
//bool s_to_l(const std::string& s, long int& l)
//{
//	char* end;
//	l = std::strtol(s.c_str(), &end, 10);
//	return s_to_test(s.c_str(), end);
//}
//
//inline
//bool s_to_ll(const std::string& s, long long int& ll)
//{
//	char* end;
//	ll = std::strtoll(s.c_str(), &end, 10);
//	return s_to_test(s.c_str(), end);
//}
//
//inline
//bool s_to_ul(const std::string& s, unsigned long int& ul)
//{
//	char* end;
//	ul = std::strtoul(s.c_str(), &end, 10);
//	return s_to_test(s.c_str(), end);
//}
//
//inline
//bool s_to_ull(const std::string& s, unsigned long long int& ull)
//{
//	char* end;
//	ull = std::strtoull(s.c_str(), &end, 10);
//	return s_to_test(s.c_str(), end);
//}
//
//inline
//bool s_to_f(const std::string& s, float& f)
//{
//	char* end;
//	f = std::strtof(s.c_str(), &end);
//	return s_to_test(s.c_str(), end);
//}
//
//inline
//bool s_to_d(const std::string& s, double& d)
//{
//	char* end;
//	d = std::strtod(s.c_str(), &end);
//	return s_to_test(s.c_str(), end);
//}
//
//inline
//bool s_to_ld(const std::string& s, long double& ld)
//{
//	char* end;
//	ld = std::strtold(s.c_str(), &end);
//	return s_to_test(s.c_str(), end);
//}

inline
long strtol_safe(const char* ptr, const char*& end, int base)
{
	return std::strtol(ptr, const_cast<char**>(&end), base);
}

// --------------------------------------------------------------------

//inline
//std::string load_file(const std::string& filepath)
//{
//	std::ifstream ifs(filepath, std::ios::binary);
//
//	if(!ifs)
//		throw std::runtime_error(std::strerror(errno));
//
//	char buf[1024];
//	std::string s;
//	while(ifs.read(buf, sizeof(buf)))
//		s.append(buf,  ifs.gcount());
//
//	return s;
//}

template<std::size_t N, typename CharT = char, typename Alloc = std::allocator<CharT>>
std::vector<CharT, Alloc> load_file(std::string const& filepath)
{
	std::basic_ifstream<CharT> ifs(filepath, std::ios::binary|std::ios::ate);

	if(!ifs)
		throw std::runtime_error(std::strerror(errno));

	std::vector<CharT, Alloc> v;
	v.reserve(ifs.tellg());
	ifs.seekg(0);

	for(CharT buf[N]; ifs.read(buf, N);)
		v.insert(v.end(), buf,  buf + ifs.gcount());

	return v;
}

template<typename CharT = char, typename Alloc = std::allocator<CharT>>
std::vector<CharT, Alloc> load_file(std::string const& filepath)
{
	return load_file<2048, CharT,Alloc>(filepath);
}

//std::string to_utf8(std::wstring w)
//{
//    return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(w);
//}
//
//std::wstring from_utf8(std::string s)
//{
//    return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().from_bytes(s);
//}

/**
 * Self-erasing buffer
 */
class secret
{
	std::vector<char> buff;

	static void opaque_fill(char* data, std::size_t size)
	{
		std::fill(data, data + size, '\0');
	}

	// call through function pointer can't be optimized away
	// because it can't be inlined
	void (*fill)(char*, std::size_t) = &opaque_fill;

public:
	secret(std::size_t size): buff(size, '\0') {}
	~secret() { fill(data(), size()); }

	char* data() { return buff.data(); }
	std::size_t size() { return buff.size(); }
};

// buffer-safe user input with std::strings
inline
std::istream& getline(std::istream& is, std::string& s, std::streamsize num, char delim = '\n')
{
	std::vector<char> buf(num + 1);
	if(is.getline(buf.data(), buf.size(), delim))
		s.assign(buf.data(), is.gcount() - 1);
	return is;
}


} // hol

#endif // HOL_STRING_UTILS_H
