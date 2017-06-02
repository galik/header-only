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

#include <set>
#include <regex>
#include <cerrno>
#include <string>
#include <vector>
#include <array>
#include <locale>
#include <numeric>
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

namespace header_only_library {
namespace string_utils {

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

namespace detail {

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

template<typename CharT>
std::basic_string<CharT>& lower_mute(std::basic_string<CharT>& s)
{
	std::transform(s.begin(), s.end(), s.begin()
		, [&](auto c){ return std::tolower(c); });
	return s;
}

template<typename CharT>
std::basic_string<CharT>& upper_mute(std::basic_string<CharT>& s)
{
	std::transform(s.begin(), s.end(), s.begin()
		, [&](auto c){ return std::toupper(c); });
	return s;
}

// trim

constexpr char const* ws(char) { return " \t\n\r\f\v\0"; }
constexpr wchar_t const* ws(wchar_t) { return L" \t\n\r\f\v\0"; }
constexpr char16_t const* ws(char16_t) { return u" \t\n\r\f\v\0"; }
constexpr char32_t const* ws(char32_t) { return U" \t\n\r\f\v\0"; }

constexpr char const* empty(char) { return ""; }
constexpr wchar_t const* empty(wchar_t) { return L""; }
constexpr char16_t const* empty(char16_t) { return u""; }
constexpr char32_t const* empty(char32_t) { return U""; }

constexpr char const* space(char) { return " "; }
constexpr wchar_t const* space(wchar_t) { return L" "; }
constexpr char16_t const* space(char16_t) { return u" "; }
constexpr char32_t const* space(char32_t) { return U" "; }

} // namespace detail

//--------------------------------------------------------------
// replace_all_mute
//
inline
std::string& replace_all_mute(std::string& s,
	const std::string& from, const std::string& to)
{
	return detail::replace_all_mute<char>(s, from, to);
}

inline
std::wstring& replace_all_mute(std::wstring& s,
	const std::wstring& from, const std::wstring& to)
{
	return detail::replace_all_mute<wchar_t>(s, from, to);
}

inline
std::u16string& replace_all_mute(std::u16string& s,
	const std::u16string& from, const std::u16string& to)
{
	return detail::replace_all_mute<char16_t>(s, from, to);
}

inline
std::u32string& replace_all_mute(std::u32string& s,
	const std::u32string& from, const std::u32string& to)
{
	return detail::replace_all_mute<char32_t>(s, from, to);
}

//--------------------------------------------------------------
// replace_all_copy
//

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

inline
std::u16string replace_all_copy(std::u16string s,
	const std::u16string& from, const std::u16string& to)
{
	return replace_all_mute(s, from, to);
}

inline
std::u32string replace_all_copy(std::u32string s,
	const std::u32string& from, const std::u32string& to)
{
	return replace_all_mute(s, from, to);
}

// -----------------------------------------------
// lower_mute
//

inline
std::string& lower_mute(std::string& s)
{
	return detail::lower_mute(s);
}

inline
std::wstring& lower_mute(std::wstring& s)
{
	return detail::lower_mute(s);
}

inline
std::u16string& lower_mute(std::u16string& s)
{
	return detail::lower_mute(s);
}

inline
std::u32string& lower_mute(std::u32string& s)
{
	return detail::lower_mute(s);
}

// -----------------------------------------------
// lower_copy
//

inline
std::string lower_copy(std::string s)
{
	return lower_mute(s);
}

inline
std::wstring lower_copy(std::wstring s)
{
	return lower_mute(s);
}

inline
std::u16string lower_copy(std::u16string s)
{
	return lower_copy(s);
}

inline
std::u32string lower_copy(std::u32string s)
{
	return lower_copy(s);
}

// -----------------------------------------------
// upper_mute
//

inline
std::string& upper_mute(std::string& s)
{
	return detail::upper_mute(s);
}

inline
std::wstring& upper_mute(std::wstring& s)
{
	return detail::upper_mute(s);
}

inline
std::u16string& upper_mute(std::u16string& s)
{
	return detail::upper_mute(s);
}

inline
std::u32string& upper_mute(std::u32string& s)
{
	return detail::upper_mute(s);
}

// -----------------------------------------------
// upper_copy
//

inline
std::string upper_copy(std::string s)
{
	return upper_mute(s);
}

inline
std::wstring upper_copy(std::wstring s)
{
	return upper_mute(s);
}

inline
std::u16string upper_copy(std::u16string s)
{
	return upper_mute(s);
}

inline
std::u32string upper_copy(std::u32string s)
{
	return upper_mute(s);
}

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
using u16output_separator = basic_output_separator<char16_t>;
using u32output_separator = basic_output_separator<char32_t>;

// trimming functions

//---------------------------------------------------------
// trim_mute
//

template<typename C, typename T, typename A>
using String = std::basic_string<C, T, A>;

/**
 * Remove leading characters from a String.
 * @param s The String to be trimmed.
 * @param ws The set of characters to remove from the beginning
 * of the String.
 * @return The same String passed in as a parameter.
 */
template<typename C, typename T = std::char_traits<C>, typename A = std::allocator<C>>
String<C, T, A>& trim_left_mute(String<C, T, A>& s, C const* ws)
{
	s.erase(0, s.find_first_not_of(ws));
	return s;
}

template<typename C, typename T = std::char_traits<C>, typename A = std::allocator<C>>
String<C, T, A>& trim_left_mute(String<C, T, A>& s, String<C, T, A> const& ws)
{
	return trim_left_mute(s, ws.c_str());
}

template<typename C, typename T = std::char_traits<C>, typename A = std::allocator<C>>
String<C, T, A>& trim_left_mute(String<C, T, A>& s)
{
	return trim_left_mute(s, detail::ws(C()));
}

/**
 * Remove trailing characters from a String.
 * @param s The String to be trimmed.
 * @param ws The set of characters to remove from the end
 * of the String.
 * @return The same String passed in as a parameter.
 */
template<typename C, typename T = std::char_traits<C>, typename A = std::allocator<C>>
String<C, T, A>& trim_right_mute(String<C, T, A>& s, C const* ws)
{
	s.erase(s.find_last_not_of(ws) + 1);
	return s;
}

template<typename C, typename T = std::char_traits<C>, typename A = std::allocator<C>>
String<C, T, A>& trim_right_mute(String<C, T, A>& s, String<C, T, A> const& ws)
{
	return trim_right_mute(s, ws.c_str());
}

template<typename C, typename T = std::char_traits<C>, typename A = std::allocator<C>>
String<C, T, A>& trim_right_mute(String<C, T, A>& s)
{
	return trim_right_mute(s, detail::ws(C()));
}

/**
 * Remove surrounding characters from a String.
 * @param s The String to be trimmed.
 * @param ws The set of characters to remove from each end
 * of the String.
 * @return The same String passed in as a parameter.
 */
template<typename C, typename T = std::char_traits<C>, typename A = std::allocator<C>>
String<C, T, A>& trim_mute(String<C, T, A>& s, C const* ws)
{
	return trim_left_mute(trim_right_mute(s, ws), ws);
}

template<typename C, typename T = std::char_traits<C>, typename A = std::allocator<C>>
String<C, T, A>& trim_mute(String<C, T, A>& s, String<C, T, A> const& ws)
{
	return trim_mute(s, ws.c_str());
}

template<typename C, typename T = std::char_traits<C>, typename A = std::allocator<C>>
String<C, T, A>& trim_mute(String<C, T, A>& s)
{
	return trim_mute(s, detail::ws(C()));
}

//---------------------------------------------------------
// trim_copy
//

/**
 * Obtain a copy of a string with leading characters removed.
 * @param s The std::string to be trimmed.
 * @param t The set of characters to remove from the beginning
 * of the String.
 * @return A copy of the string passed in as a parameter.
 */
template<typename C, typename T = std::char_traits<C>, typename A = std::allocator<C>>
String<C, T, A> trim_left_copy(String<C, T, A> s, C const* ws)
{
	return trim_left_mute(s, ws);
}

template<typename C, typename T = std::char_traits<C>, typename A = std::allocator<C>>
String<C, T, A> trim_left_copy(String<C, T, A> s, String<C, T, A> const& ws)
{
	return trim_left_mute(s, ws);
}

template<typename C, typename T = std::char_traits<C>, typename A = std::allocator<C>>
String<C, T, A>& trim_left_copy(String<C, T, A> s)
{
	return trim_left_mute(s);
}

/**
 * Obtain a copy of a string with trailing characters removed.
 * @param s The std::string to be trimmed.
 * @param t The set of characters to remove from the end
 * of the String.
 * @return A copy of the string passed in as a parameter.
 */
template<typename C, typename T = std::char_traits<C>, typename A = std::allocator<C>>
String<C, T, A> trim_right_copy(String<C, T, A> s, C const* ws)
{
	return trim_right_mute(s, ws);
}

template<typename C, typename T = std::char_traits<C>, typename A = std::allocator<C>>
String<C, T, A> trim_right_copy(String<C, T, A> s, String<C, T, A> const& ws)
{
	return trim_right_mute(s, ws);
}

template<typename C, typename T = std::char_traits<C>, typename A = std::allocator<C>>
String<C, T, A> trim_right_copy(String<C, T, A> s)
{
	return trim_right_mute(s);
}

/**
 * Obtain a copy of a String with surrounding characters removed.
 * @param s The String to be trimmed.
 * @param t The set of characters to remove from each end
 * of the Sring.
 * @return A copy of the String passed in as a parameter.
 */
template<typename C, typename T = std::char_traits<C>, typename A = std::allocator<C>>
String<C, T, A> trim_copy(String<C, T, A> s, C const* ws)
{
	return trim_mute(s, ws);
}

template<typename C, typename T = std::char_traits<C>, typename A = std::allocator<C>>
String<C, T, A> trim_copy(String<C, T, A> s, String<C, T, A> const& ws)
{
	return trim_mute(s, ws);
}

template<typename C, typename T = std::char_traits<C>, typename A = std::allocator<C>>
String<C, T, A> trim_copy(String<C, T, A> s)
{
	return trim_mute(s);
}

//---------------------------------------------------------
// trim_keep
//

/**
 * Remove (and keep) leading characters from a String.
 * @param s The String to be trimmed.
 * @param ws The set of characters to remove from the beginning
 * of the String.
 * @return The String of characters that were removed.
 */
template<typename C, typename T = std::char_traits<C>, typename A = std::allocator<C>>
String<C, T, A> trim_left_keep(String<C, T, A> s, C const* ws)
{
	typename String<C, T, A>::size_type pos;
	String<C, T, A> keep = s.substr(0, (pos = s.find_first_not_of(ws)));
	s.erase(0, pos);
	return keep;
}

template<typename C, typename T = std::char_traits<C>, typename A = std::allocator<C>>
String<C, T, A> trim_left_keep(String<C, T, A> s, String<C, T, A> const& ws)
{
	return trim_left_keep(s, ws.c_str());
}

template<typename C, typename T = std::char_traits<C>, typename A = std::allocator<C>>
String<C, T, A> trim_left_keep(String<C, T, A> s)
{
	return trim_left_keep(s, detail::ws(C()));
}

/**
 * Remove (and keep) trailing characters from a String.
 * @param s The String to be trimmed.
 * @param ws The set of characters to remove from the end
 * of the String.
 * @return The String of characters that were removed.
 */
template<typename C, typename T = std::char_traits<C>, typename A = std::allocator<C>>
String<C, T, A> trim_right_keep(String<C, T, A> s, C const* ws)
{
	typename String<C, T, A>::size_type pos;
	String<C, T, A> keep = s.substr((pos = s.find_last_not_of(ws) + 1));
	s.erase(pos);
	return keep;
}

template<typename C, typename T = std::char_traits<C>, typename A = std::allocator<C>>
String<C, T, A> trim_right_keep(String<C, T, A> s, String<C, T, A> const& ws)
{
	return trim_right_keep(s, ws.c_str());
}

template<typename C, typename T = std::char_traits<C>, typename A = std::allocator<C>>
String<C, T, A> trim_right_keep(String<C, T, A> s)
{
	return trim_right_keep(s, detail::ws(C()));
}

/**
 * Remove (and keep) leading and trailing characters from a String.
 * @param s The String to be trimmed.
 * @param ws The set of characters to remove from each end
 * of the String.
 * @return A structure of the form `struct { String left; String right; };`
 * containing each String of characters that was removed.
 */
template<typename C, typename T = std::char_traits<C>, typename A = std::allocator<C>>
auto trim_keep(String<C, T, A> s, C const* ws)
{
	struct rv
	{
		String<C, T, A> left;
		String<C, T, A> right;
	};

	return rv{trim_left_keep(s, ws), trim_right_keep(s, ws)};
}

template<typename C, typename T = std::char_traits<C>, typename A = std::allocator<C>>
auto trim_keep(String<C, T, A> s, String<C, T, A> const& ws)
{
	return trim_keep(s, ws.c_str());
}

template<typename C, typename T = std::char_traits<C>, typename A = std::allocator<C>>
auto trim_keep(String<C, T, A> s)
{
	return trim_keep(s, detail::ws(C()));
}

////---------------------------------------------------------
//// trim_mute
////
//
///**
// * Remove leading characters from a String.
// * @param s The String to be trimmed.
// * @param ws The set of characters to remove from the beginning
// * of the String.
// * @return The same String passed in as a parameter.
// */
//template<typename String>
//String& trim_left_mute(String& s, typename String::value_type const* ws)
//{
//	s.erase(0, s.find_first_not_of(ws));
//	return s;
//}
//
//template<typename String>
//String& trim_left_mute(String& s, String const& ws)
//{
//	return trim_left_mute(s, ws.c_str());
//}
//
//template<typename String>
//String& trim_left_mute(String& s)
//{
//	return trim_left_mute(s, detail::ws(typename String::value_type()));
//}
//
///**
// * Remove trailing characters from a String.
// * @param s The String to be trimmed.
// * @param ws The set of characters to remove from the end
// * of the String.
// * @return The same String passed in as a parameter.
// */
//template<typename String>
//String& trim_right_mute(String& s, typename String::value_type const* ws)
//{
//	s.erase(s.find_last_not_of(ws) + 1);
//	return s;
//}
//
//template<typename String>
//String& trim_right_mute(String& s, String const& ws)
//{
//	return trim_right_mute(s, ws.c_str());
//}
//
//template<typename String>
//String& trim_right_mute(String& s)
//{
//	return trim_right_mute(s, detail::ws(typename String::value_type()));
//}
//
///**
// * Remove surrounding characters from a String.
// * @param s The String to be trimmed.
// * @param ws The set of characters to remove from each end
// * of the String.
// * @return The same String passed in as a parameter.
// */
//template<typename String>
//String& trim_mute(String& s, typename String::value_type const* ws)
//{
//	return trim_left_mute(trim_right_mute(s, ws), ws);
//}
//
//template<typename String>
//String& trim_mute(String& s, String const& ws)
//{
//	return trim_mute(s, ws.c_str());
//}
//
//template<typename String>
//String& trim_mute(String& s)
//{
//	return trim_mute(s, detail::ws(typename String::value_type()));
//}
//
////---------------------------------------------------------
//// trim_copy
////
//
///**
// * Obtain a copy of a string with leading characters removed.
// * @param s The std::string to be trimmed.
// * @param t The set of characters to remove from the beginning
// * of the String.
// * @return A copy of the string passed in as a parameter.
// */
//template<typename String>
//String trim_left_copy(String s, typename String::value_type const* ws)
//{
//	return trim_left_mute(s, ws);
//}
//
//template<typename String>
//String trim_left_copy(String s, String const& ws)
//{
//	return trim_left_mute(s, ws);
//}
//
//template<typename String>
//String& trim_left_copy(String s)
//{
//	return trim_left_mute(s);
//}
//
///**
// * Obtain a copy of a string with trailing characters removed.
// * @param s The std::string to be trimmed.
// * @param t The set of characters to remove from the end
// * of the String.
// * @return A copy of the string passed in as a parameter.
// */
//template<typename String>
//String trim_right_copy(String s, typename String::value_type const* ws)
//{
//	return trim_right_mute(s, ws);
//}
//
//template<typename String>
//String trim_right_copy(String s, String const& ws)
//{
//	return trim_right_mute(s, ws);
//}
//
//template<typename String>
//String trim_right_copy(String s)
//{
//	return trim_right_mute(s);
//}
//
///**
// * Obtain a copy of a String with surrounding characters removed.
// * @param s The String to be trimmed.
// * @param t The set of characters to remove from each end
// * of the Sring.
// * @return A copy of the String passed in as a parameter.
// */
//template<typename String>
//String trim_copy(String s, typename String::value_type const* ws)
//{
//	return trim_mute(s, ws);
//}
//
//template<typename String>
//String trim_copy(String s, String const& ws)
//{
//	return trim_mute(s, ws);
//}
//
//template<typename String>
//String trim_copy(String s)
//{
//	return trim_mute(s);
//}
//
////---------------------------------------------------------
//// trim_keep
////
//
///**
// * Remove (and keep) leading characters from a String.
// * @param s The String to be trimmed.
// * @param ws The set of characters to remove from the beginning
// * of the String.
// * @return The String of characters that were removed.
// */
//template<typename String>
//String trim_left_keep(String s, typename String::value_type const* ws)
//{
//	typename String::size_type pos;
//	String keep = s.substr(0, (pos = s.find_first_not_of(ws)));
//	s.erase(0, pos);
//	return keep;
//}
//
//template<typename String>
//String trim_left_keep(String s, String const& ws)
//{
//	return trim_left_keep(s, ws.c_str());
//}
//
//template<typename String>
//String trim_left_keep(String s)
//{
//	return trim_left_keep(s, detail::ws(typename String::value_type()));
//}
//
///**
// * Remove (and keep) trailing characters from a String.
// * @param s The String to be trimmed.
// * @param ws The set of characters to remove from the end
// * of the String.
// * @return The String of characters that were removed.
// */
//template<typename String>
//String trim_right_keep(String s, typename String::value_type const* ws)
//{
//	typename String::size_type pos;
//	String keep = s.substr((pos = s.find_last_not_of(ws) + 1));
//	s.erase(pos);
//	return keep;
//}
//
//template<typename String>
//String trim_right_keep(String s, String const& ws)
//{
//	return trim_right_keep(s, ws.c_str());
//}
//
//template<typename String>
//String trim_right_keep(String s)
//{
//	return trim_right_keep(s, detail::ws(typename String::value_type()));
//}
//
///**
// * Remove (and keep) leading and trailing characters from a String.
// * @param s The String to be trimmed.
// * @param ws The set of characters to remove from each end
// * of the String.
// * @return A structure of the form `struct { String left; String right; };`
// * containing each String of characters that was removed.
// */
//template<typename String>
//auto trim_keep(String s, typename String::value_type const* ws)
//{
//	struct rv
//	{
//		String left;
//		String right;
//	};
//
//	return rv{trim_left_keep(s, ws), trim_right_keep(s, ws)};
//}
//
//template<typename String>
//auto trim_keep(String s, String const& ws)
//{
//	return trim_keep(s, ws.c_str());
//}
//
//template<typename String>
//auto trim_keep(String s)
//{
//	return trim_keep(s, detail::ws(typename String::value_type()));
//}

// GSL_STRING_SPAN

#ifdef HOL_USE_STRING_SPAN
namespace gsl_detail {
namespace {
gsl::cstring_span<> ws{" \t\n\r\f\v\0"};//(::header_only_library::string_utils::detail::ws(char()));
}

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

//template<typename Container>
//std::string join(const Container& c, std::string const& delim = " ")
//{
//	std::string ret, sep;
//	for(auto const& s: c)
//		{ ret += sep + s; sep = delim; }
//	return ret;
//}

template<class CharT, template<class> class String, template<class> class Container>
String<CharT> join(Container<String<CharT>> const& c, String<CharT> const& delim)
{
	String<CharT> ret, sep;

	auto size = std::accumulate(std::begin(c), std::end(c), std::size_t(0), [](auto n, auto const& s)
	{
		return n + s.size();
	}) + ((c.size() - 1) * delim.size());

	ret.reserve(size);

	for(auto const& s: c)
		{ ret += sep + s; sep = delim; }
	return ret;
}

template<class CharT, template<class> class String, template<class> class Container>
String<CharT> join(Container<String<CharT>> const& c, CharT const* delim)
{
	return join(c, String<CharT>(delim));
}

template<class CharT, template<class> class String, template<class> class Container>
String<CharT> join(Container<String<CharT>> const& c)
{
	return join(c, String<CharT>(detail::empty(CharT())));
}

// --------------------------------------------------------------------
// string conversions
// --------------------------------------------------------------------

// All these conversions return true only if a valid
// number surrounded only by spaces is found

// they accept the same input as their corresponding
// std::strtoxx() functions.

bool s_to_l(const std::string& s, long& l);
bool s_to_ll(const std::string& s, long long& ll);

bool s_to_ul(const std::string& s, unsigned long& ul);
bool s_to_ull(const std::string& s, unsigned long long& ull);

bool s_to_f(const std::string& s, float& f);
bool s_to_d(const std::string& s, double& d);
bool s_to_ld(const std::string& s, long double& ld);

// string conversions

inline
bool s_to_test(char const* s, char const* e)
{
	if(e == s)
		return false;

	while(std::isspace(*e))
		++e;

	return !(*e);
}

inline
bool s_to_l(const std::string& s, long int& l)
{
	char* end;
	l = std::strtol(s.c_str(), &end, 10);
	return s_to_test(s.c_str(), end);
}

inline
bool s_to_ll(const std::string& s, long long int& ll)
{
	char* end;
	ll = std::strtoll(s.c_str(), &end, 10);
	return s_to_test(s.c_str(), end);
}

inline
bool s_to_ul(const std::string& s, unsigned long int& ul)
{
	char* end;
	ul = std::strtoul(s.c_str(), &end, 10);
	return s_to_test(s.c_str(), end);
}

inline
bool s_to_ull(const std::string& s, unsigned long long int& ull)
{
	char* end;
	ull = std::strtoull(s.c_str(), &end, 10);
	return s_to_test(s.c_str(), end);
}

inline
bool s_to_f(const std::string& s, float& f)
{
	char* end;
	f = std::strtof(s.c_str(), &end);
	return s_to_test(s.c_str(), end);
}

inline
bool s_to_d(const std::string& s, double& d)
{
	char* end;
	d = std::strtod(s.c_str(), &end);
	return s_to_test(s.c_str(), end);
}

inline
bool s_to_ld(const std::string& s, long double& ld)
{
	char* end;
	ld = std::strtold(s.c_str(), &end);
	return s_to_test(s.c_str(), end);
}

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

template<std::size_t N, typename CharT = char>
std::basic_string<CharT> load_file(std::string const& filepath)
{
	std::basic_ifstream<CharT> ifs(filepath, std::ios::binary|std::ios::ate);

	if(!ifs)
		throw std::runtime_error(filepath + ": " + std::strerror(errno));

	auto end = ifs.tellg();
	ifs.seekg(0, std::ios::beg);

	std::basic_string<CharT> s;
	s.resize(std::size_t(end - ifs.tellg()));

	if(!ifs.read(&s[0], s.size()))
		throw std::runtime_error(filepath + ": " + std::strerror(errno));

	return s;
}

template<typename CharT = char>
std::basic_string<CharT> load_file(std::string const& filepath)
{
	return load_file<2048, CharT>(filepath);
}

/**
 * Self-erasing buffer
 */
template<std::size_t N>
class erasing_buffer
{
public:
	erasing_buffer() noexcept: buff({}) {}
	~erasing_buffer() { fill(data()); }

	char volatile* data() { return buff.data(); }
	char volatile const* data() const { return buff.data(); }

	std::size_t size() const { return buff.size(); }

	auto begin() { return std::begin(buff); }
	auto begin() const { return std::begin(buff); }

	auto end() { return std::end(buff); }
	auto end() const { return std::end(buff); }

private:
	static void opaque_fill(char volatile* data)
		{ std::fill(data, data + N, '\0'); }

	// call through function pointer can't be optimized away
	// because it can't be inlined
	void (*fill)(char volatile*) = &opaque_fill;
	std::array<char volatile, N> buff;
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

class mapped_stencil
{
	struct place
	{
		using set = std::set<place>;
		using vec = std::vector<place>;

		char const* pos;
		std::size_t len;

		place(char const* pos, std::size_t len): pos(pos), len(len) {}

		bool operator<(place const& other) const { return pos < other.pos; }
	};

	// order: piece, var, piece, var, piece
	place::set vars;
	std::string text;

public:
	using dict = std::map<std::string, std::string>;

	/**
	 * Create an empty stencil
	 */
	mapped_stencil() {}
	mapped_stencil(mapped_stencil&& s): vars(std::move(s.vars)), text(std::move(s.text)) {}
	mapped_stencil(const mapped_stencil& s): vars(s.vars), text(s.text) {}

	mapped_stencil& operator=(mapped_stencil&& s) { vars = std::move(s.vars); text = std::move(s.text); return *this; }
	mapped_stencil& operator=(mapped_stencil const& s) { vars = s.vars; text = s.text; return *this; }

	void clear() { vars.clear(); text.clear(); }

	void compile_file(std::string const& filename, std::set<std::string> const& vars)
	{
		std::stringstream ss;
		ss << std::ifstream(filename).rdbuf();
		return compile(ss.str(), vars);
	}

	void compile(std::string const& text, std::set<std::string> const& vars)
	{
		clear();

		this->text = text;

		for(auto const& v: vars)
			for(auto pos = 0ULL; (pos = text.find(v, pos)) != std::string::npos; pos += v.size())
				this->vars.emplace(this->text.data() + pos, v.size());

		// embedded vars found in order
	}

	std::string create(const dict& d) const
	{
		if(vars.empty())
			return text;

		std::string out;
		out.reserve(text.size());

		auto pos = text.data();

		for(auto const& v: vars)
		{
			if(v.pos > pos)
				out.append(pos, v.pos);

			auto found = d.find({v.pos, v.len});

			if(found != d.end())
				out.append(found->second);

			pos = v.pos + v.len;
		}

		if(pos < text.data() + text.size())
			out.append(pos, text.size());

		return out;
	}
};

//namespace utf8 {
//
//inline std::string from_ws(std::wstring const& s)
//{
//	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cnv;
//	std::string utf8 = cnv.to_bytes(s);
//	if(cnv.converted() < s.size())
//		throw std::runtime_error("incomplete conversion");
//	return utf8;
//}
//
//inline std::wstring to_ws(std::string const& utf8)
//{
//	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cnv;
//	std::wstring s = cnv.from_bytes(utf8);
//	if(cnv.converted() < utf8.size())
//		throw std::runtime_error("incomplete conversion");
//	return s;
//}
//
//} // namespace utf8
} // string_utils
} // header_only_library

#endif // HOL_STRING_UTILS_H
