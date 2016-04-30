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
#include <iomanip>
#include <iostream>
#include <cstring>
#include <cctype>
#include <algorithm>

#include "test.h"
//#include "hol/string_utils.h"
#include "hol/u8string.h"

namespace hol {

using std::string;
using std::vector;
using std::experimental::string_view;

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
	using string_type = std::experimental::string_view;
	using return_type = std::string;
	using vector_type = std::vector<return_type>;
};

struct view
{
	using string_type = std::experimental::string_view;
	using return_type = string_type;
	using vector_type = std::vector<string_type>;
};

//struct span
//{
//	using string_type = gsl::string_span<>;
//	using return_type = string_type;
//	using vector_type = std::vector<string_type>;
//};

template<typename CharT>
typename std::basic_string<CharT>::size_type
find_first_not_of(std::basic_string<CharT> const& s
	, std::vector<CharT> chars
	, typename std::basic_string<CharT>::size_type pos)
{
	return s.find_first_not_of(chars.data(), pos, chars.size());
}

hol::u8string::size_type
find_first_not_of(hol::u8string const& s
	, std::vector<u8char> chars
	, hol::u8string::size_type pos)
{
//	return s.find_first_not_of(chars.data(), pos, chars.size());
}

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
 *
 * Mode = view (viewing)
 *     Accepts a string_view parameter and returns a string_view
 *     that is a copy of the passed in string_view, with the
 *     configured characters removed.
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
		|| std::is_same<Mode, view>::value
//		|| std::is_same<Mode, span>::value
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
};

using trim_mutator = trimmer<mute>;
using trim_copier = trimmer<copy>;
using trim_viewer = trimmer<view>;
//using trim_spanner = trimmer<span>;

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

private:
	bool fold = true;
	std::size_t reserve = 20;

	template<typename Iterator, typename Comparator>
	vector_type at(Iterator pos, Iterator done, Comparator cmp)
	{
		vector_type v;
		v.reserve(reserve);

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

		return v;
	}

	template<typename Comparator>
	vector_type at(string_type s, Comparator cmp)
	{
		return at(s.data(), s.data() + s.size(), cmp);
	}

public:
	void fold_matches() { fold = true; }
	void dont_fold_matches() { fold = false; }
	void reserve_at_least(std::size_t reserve) { this->reserve = reserve; }

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
		vector_type v;
		v.reserve(reserve);

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

	vector_type at_regex(string_type s, std::regex e)
	{
		vector_type v;
		v.reserve(reserve);

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

} // hol

int main()
{
	try
	{
	}
	catch(const std::exception& e)
	{
		ERR(e.what());
	}
	catch(...)
	{
		ERR("Unknown exception thrown");
	}
}

