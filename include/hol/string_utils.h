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

// TODO: remove this when condition is
// fully appled to all uses
#define HOL_USE_STRING_VIEW

#include <regex>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <algorithm>
#if defined(HOL_USE_STRING_VIEW)
#	include <experimental/string_view>
#endif

#include "stl.h"

namespace hol {

using std::string;
using std::vector;
#if defined(HOL_USE_STRING_VIEW)
using std::experimental::string_view;
#endif

inline
std::string& replace_all(std::string& s, const std::string& from, const std::string& to)
{
	if(!from.empty())
		for(size_t pos = 0; (pos = s.find(from, pos)) != std::string::npos; pos += to.size())
			s.replace(pos, from.size(), to);
	return s;
}

// string operator modes

struct mute
{
	using string_type = std::string&;
	using return_type = string_type;
};

struct copy
{
#if defined(HOL_USE_STRING_VIEW)
	using string_type = std::experimental::string_view;
#else
	using string_type = std::string const&;
#endif
	using return_type = std::string;
	using vector_type = std::vector<return_type>;
};

#if defined(HOL_USE_STRING_VIEW)
struct view
{
	using string_type = std::experimental::string_view;
	using return_type = string_type;
	using vector_type = std::vector<string_type>;
};
#endif

#if defined(HOL_USE_GSL_SPAN)
struct span
{
	using string_type = gsl::string_span<>;
	using return_type = string_type;
	using vector_type = std::vector<string_type>;
};
#endif

/**
 * Generic class for trimming characters from the left and
 * right hand sides of a string. This class works differently
 * with different Mode parameters.
 * <pre>
 * Mode = mute (mutating)
 *     Accepts a std::string& parameter and modifies the
 *     passed in std::string, returning a reference to the
 *     same string.
 *
 * Mode = copy (copying)
 *     Accepts a string_view parameter and returns a std::string
 *     that is a copy of the passed in string_view, with the
 *     configured characters removed.
 */
#if defined(HOL_USE_STRING_VIEW)
 /**
  * Mode = view (viewing)
  *     Accepts a string_view parameter and returns a string_view
  *     that is a copy of the passed in string_view, with the
  *     configured characters removed.
  */
#endif
#if defined(HOL_USE_GSL_SPAN)
 /**
  * Mode = span (spanning)
  *     Accepts a gsl::string_span parameter and returns a string_span
  *     that is a copy of the passed in string_span, with the
  *     configured characters removed.
  */
#endif
/**
 *
 * </pre>
 */
template<typename Mode>
class trimmer
{
	static_assert
	(
		   std::is_same<Mode, mute>::value
		|| std::is_same<Mode, copy>::value
#if defined(HOL_USE_STRING_VIEW)
		|| std::is_same<Mode, view>::value
#endif
#if defined(HOL_USE_GSL_SPAN)
		|| std::is_same<Mode, span>::value
#endif
		, "Mode template parameter must be mute, copy, view or span"
	);

public:
	using mode = Mode;
	using string_type = typename mode::string_type;
	using return_type = typename mode::return_type;

private:
	static constexpr const char* const ws = " \t\n\r\f\v";
	string shed_l = ws;
	string shed_r = ws;

	return_type left(string_type s, mute) const
	{
		s.erase(0, s.find_first_not_of(shed_l.c_str()));
		return s;
	}

	return_type left(string_type s, view) const
	{
		if(auto pos = s.find_first_not_of(shed_l.c_str()) + 1)
		{
			s.remove_prefix(pos - 1);
			return {s.data(), s.size()};
		}
		return {};
	}

	return_type left(string_type s, copy) const
	{
		return left(s, view());
	}

	return_type right(string_type s, mute) const
	{
		s.erase(s.find_last_not_of(shed_r.c_str()) + 1);
		return s;
	}

	return_type right(string_type s, view) const
	{
		if(auto pos = s.find_last_not_of(shed_r.c_str()) + 1)
			s.remove_suffix(s.size() - pos);
		return {s.data(), s.size()};
	}

	return_type right(string_type s, copy) const
	{
		return right(s, view());
	}

public:

	/**
	 * Configure which characters will be removed from both the
	 * left and the right of the string.
	 * @param characters The characters to be removed.
	 */
	void remove(string const& characters) { shed_l = shed_r = characters; }

	/**
	 * Configure which characters will be removed from the
	 * left the string.
	 * @param characters The characters to be removed.
	 */
	void remove_from_left(string const& characters) { shed_l = characters; }

	/**
	 * Configure which characters will be removed from the
	 * right the string.
	 * @param characters The characters to be removed.
	 */
	void remove_from_right(string const& characters) { shed_r = characters; }

	/**
	 * Remove characters from the left of the string.
	 * @param s The string to remove characters from.
	 * @return The string with trimmed characters removed.
	 */
	return_type left(string_type s) const { return left(s, Mode()); }

	/**
	 * Remove characters from the right of the string.
	 * @param s The string to remove characters from.
	 * @return The string with trimmed characters removed.
	 */
	return_type right(string_type s) const { return right(s, Mode()); }

	/**
	 * Remove characters from the left and the right of the string.
	 * @param s The string to remove characters from.
	 * @return The string with trimmed characters removed.
	 */
	return_type round(string_type s) const { return left(right(s, Mode()), Mode()); }

	return_type operator()(string_type s) const { return round(s); }
};

using trim_mutator = trimmer<mute>;
using trim_copier = trimmer<copy>;
#if defined(HOL_USE_STRING_VIEW)
using trim_viewer = trimmer<view>;
#endif
#if defined(HOL_USE_GSL_SPAN)
using trim_spanner = trimmer<span>;
#endif

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

template<typename Mode>
class splitter
{
	static_assert
	(
		   std::is_same<Mode, copy>::value
		|| std::is_same<Mode, view>::value
//		|| std::is_same<Mode, span>::value
		, "Mode template parameter must be copy, view or span"
	);

public:
	using mode = Mode;
	using string_type = std::experimental::string_view;
	using vector_type = typename mode::vector_type;

	struct config
	{
		bool fold = true;
		bool strict = false;
		std::size_t reserve = 20;
		void fold_matches() { fold = true; }
		void dont_fold_matches() { fold = false; }
		void reserve_at_least(std::size_t reserve) { this->reserve = reserve; }
	};

	void configure(config const& cfg)
	{
		this->cfg = cfg;
	}

	config& configure()
	{
		return cfg;
	}

	config configuration() const { return cfg; }

private:
//	bool fold = true;
//	bool strict = false;
//	std::size_t reserve = 20;

	config cfg;

	template<typename Iterator, typename Comparator>
	vector_type at(Iterator pos, Iterator done, Comparator cmp)
	{
		vector_type v;
		v.reserve(cfg.reserve);

		auto end = pos;

		if(cfg.fold)
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

		return v;
	}

	template<typename Comparator>
	vector_type at(string_type s, Comparator cmp)
	{
		return at(s.data(), s.data() + s.size(), cmp);
	}

public:
	void fold_matches() { cfg.fold_matches(); }
	void dont_fold_matches() { cfg.dont_fold_matches(); }
	void reserve_at_least(std::size_t reserve) { cfg.reserve_at_least(reserve); }

	vector_type at_space(string_type s)
	{
		return at(s, std::ptr_fun<int, int>(std::isspace));
	}

	vector_type at_delim(string_type s, char delim)
	{
		return at(s, [delim](char c){return c == delim;});
	}

	vector_type at_delims(string_type s, string_view delims)
	{
		return at(s, [delims](char c){return delims.find(c) != string_view::npos;});
	}

	vector_type at_delim(string_type s, string_view delim)
	{
		if(delim.empty())
			return {};

		vector_type v;
		v.reserve(cfg.reserve);

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

		return v;
	}

	vector_type at_regex(string_type s, std::regex const& e)
	{
		vector_type v;
		v.reserve(cfg.reserve);

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

		return v;
	}

	vector_type at_regex(string_type s, string const& e)
	{
		return at_regex(s, std::regex(e));
	}
};

using split_copier = splitter<copy>;
using split_viewer = splitter<view>;
//using split_spanner = splitter<span>;

class output_separator
{
	char const* init;
	char const* s;
	char const* const next;

public:
	output_separator(): init(""), s(init), next(" ") {}
	output_separator(char const* next): init(""), s(init), next(next) {}
	output_separator(char const* init, char const* next): init(init), s(init), next(next) {}
	output_separator(output_separator const& sep): s(sep.s), next(sep.next) {}

	friend std::ostream& operator<<(std::ostream& os, output_separator& sep)
	{
		os << sep.s;
		sep.s = sep.next;
		return os;
	}

	friend std::string operator+(std::string const& s, output_separator& sep)
	{
		std::string r = s + sep.s;
		sep.s = sep.next;
		return r;
	}

	friend std::string operator+(output_separator& sep, std::string const& s)
	{
		std::string r = sep.s + s;
		sep.s = sep.next;
		return r;
	}

	void reset() { this->s = init; }
	void reset(char const* s) { this->s = init = s; }
};

// trimming functions

constexpr const char* const ws = " \t\n\r\f\v";

/**
 * Remove leading characters from a str.
 * @param s The std::string to be modified.
 * @param t The set of characters to delete from the beginning
 * of the string.
 * @return The same string passed in as a parameter reference.
 */
inline std::string& trim_left_mute(std::string& s, const char* t = ws)
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
inline std::string& trim_right_mute(std::string& s, const char* t = ws)
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
inline std::string& trim_mute(std::string& s, const char* t = ws)
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
inline std::string trim_left_mute(std::string&& s, const char* t = ws)
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
inline std::string trim_right_mute(std::string&& s, const char* t = ws)
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
inline std::string trim_mute(std::string&& s, const char* t = ws)
{
	return trim_left_mute(trim_right_mute(s, t), t);
}

inline std::string trim_left_copy(std::string s, const char* t = ws)
{
	return trim_left_mute(s, t);
}

inline std::string trim_right_copy(std::string s, const char* t = ws)
{
	return trim_right_mute(s, t);
}

inline std::string trim_copy(std::string s, const char* t = ws)
{
	return trim_mute(s, t);
}
//views
inline string_view trim_left_view(string_view s, const char* t = ws)
{
	if(auto pos = s.find_first_not_of(t) + 1)
	{
		s.remove_prefix(pos - 1);
		return {s.data(), s.size()};
	}
	return {};
}

inline string_view trim_right_view(string_view s, const char* t = ws)
{
	if(auto pos = s.find_last_not_of(t) + 1)
		s.remove_suffix(s.size() - pos);
	return {s.data(), s.size()};
}

inline string_view trim_view(string_view s, const char* t = ws)
{
	return trim_left_view(trim_right_view(s, t), t);
}

/**
 * Remove all leading characters of a given value
 * from a str.
 * @param s The string to be modified.
 * @param c The character value to delete.
 * @return The same string passed in as a parameter reference.
 */
inline std::string& trim_left_mute(std::string& s, char c)
{
	s.erase(0, s.find_first_not_of(c));
	return s;
}

/**
 * Remove all trailing characters of a given value
 * from a str.
 * @param s The string to be modified.
 * @param c The character value to delete.
 * @return The same string passed in as a parameter reference.
 */
inline std::string& trim_right_mute(std::string& s, char c)
{
	s.erase(s.find_last_not_of(c) + 1);
	return s;
}

/**
 * Remove all surrounding characters of a given value
 * from a str.
 * @param s The string to be modified.
 * @param c The character value to delete.
 * @return The same string passed in as a parameter reference.
 */
inline std::string& trim_mute(std::string& s, char c)
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

// NEW DEFINITIVE SPLIT ALGORITHM??

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
	if(s.empty())
		return {};

	StringType delim = delim_in;

	VectorType v;
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
std::vector<std::string> split(
	std::string const& s
	, std::string const& delim = " "
	, bool fold = true
	, bool strict = false)
{
	return basic_split(s, delim, fold, strict);
}

inline
std::vector<std::wstring> split(
	std::wstring const& s
	, std::wstring const& delim = L" "
	, bool fold = true
	, bool strict = false)
{
	return basic_split(s, delim, fold, strict);
}

inline
auto make_range(std::string const& s, std::regex const& e)
{
	return hol::stl::make_range(std::sregex_iterator(s.begin(), s.end(), e), std::sregex_iterator());
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

// --------------------------------------------------------------------

inline
std::string load_file(const std::string& filepath, std::ios::openmode mode = std::ios::in)
{
	mode |= std::ios::in;
	mode &= ~std::ios::out;

	std::ifstream ifs(filepath, mode);

	if(!ifs)
		throw std::runtime_error(std::strerror(errno));

	std::stringstream ss;
	ss << ifs.rdbuf();

	return ss.str();
}

} // hol

#endif // HOL_STRING_UTILS_H
