#ifndef HOL_RANGE_H
#define HOL_RANGE_H
//
// Copyright (c) 2017 Galik <galik.bool@gmail.com>
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
#include <cassert>
#include <iostream>
#include <iterator>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

#define HOL_THROW_RANGE_EXCEPTION(msg) do{std::ostringstream o;o<<msg;throw std::runtime_error(o.str());}while(0)

#ifndef HOL_RANGE_EXCEPTION_POLICY
# ifdef HOL_RANGE_THROW_ON_EXCEPTION
#  define HOL_RANGE_EXCEPTION_POLICY(msg) HOL_THROW_RANGE_EXCEPTION(msg)
#  define HOL_RANGE_NOEXCEPT
# else
#  ifdef NDEBUG
#   define HOL_RANGE_EXCEPTION_POLICY(msg) \
	 do{std::cerr<<msg<<'\n';std::abort();}while(0)
#  else
#   define HOL_RANGE_EXCEPTION_POLICY(msg) \
	 do{std::cerr<<msg<<"\nfile: "<<__FILE__<<"\nline: "<<__LINE__<<'\n';std::abort();}while(0)
#  endif
#  define HOL_RANGE_NOEXCEPT noexcept
# endif
#endif

#ifdef NDEBUG
# ifdef HOL_RANGE_THROW_ON_EXCEPTION
#  define HOL_RANGE_ASSERT(expr) do{ if(!(expr)) HOL_RANGE_EXCEPTION_POLICY(#expr " failed."); }while(0)
# else
#  define HOL_RANGE_ASSERT(expr) do{}while(0)
# endif
#else
# define HOL_RANGE_ASSERT(expr) do{ if(!(expr)) HOL_RANGE_EXCEPTION_POLICY(#expr " failed."); }while(0)
#endif

#ifndef HOL_DEBUG_ONLY_SECTION
#ifndef NDEBUG
#define HOL_DEBUG_ONLY_SECTION(code) code
#else
#define HOL_DEBUG_ONLY_SECTION(code)
#endif
#endif // HOL_DEBUG_ONLY_SECTION

namespace header_only_library {
namespace range {
namespace detail {

template<typename T>
class basic_reverse_iterator
{
public:
	basic_reverse_iterator(T* pos): m_pos(pos) {}

	basic_reverse_iterator& operator++()    { --m_pos; return *this; }
	basic_reverse_iterator  operator++(int) { auto i = *this; ++(*this); return i; }

	basic_reverse_iterator& operator--()    { ++m_pos; return *this; }
	basic_reverse_iterator  operator--(int) { auto i = *this; --(*this); return i; }

	bool operator==(basic_reverse_iterator other) const { return m_pos == other.m_pos; }
	bool operator!=(basic_reverse_iterator other) const { return !(*this == other); }

	T& operator*() { return *base(); }
	T const& operator*() const { return *base(); }

	T* operator->() { return base(); }
	T const* operator->() const { return base(); }

	T* base() { return m_pos - 1; }

private:
	T* m_pos;
};

template<typename CharT>
std::size_t length_of(CharT const* s)
{
	auto b = s;
	while(*s) ++s;
	return std::size_t(s - b);
}

} // namespace detail

template<typename T>
class basic_range
{
public:
	using value_type = T;
	using pointer = value_type*;
	using const_pointer = value_type const*;
	using reference = value_type&;
	using const_reference = value_type const&;
	using iterator = pointer;
	using const_iterator = const_pointer;
	using reverse_iterator = detail::basic_reverse_iterator<T>;
	using const_reverse_iterator = detail::basic_reverse_iterator<T const>;

	basic_range() HOL_RANGE_NOEXCEPT = default;
	basic_range(basic_range&& r) HOL_RANGE_NOEXCEPT = default;
	basic_range(basic_range const& r) HOL_RANGE_NOEXCEPT = default;

	basic_range& operator=(basic_range&& r) HOL_RANGE_NOEXCEPT = default;
	basic_range& operator=(basic_range const& r) HOL_RANGE_NOEXCEPT = default;

	explicit basic_range(T* beg, T* end) HOL_RANGE_NOEXCEPT: m_beg(beg), m_end(end)
	{
		HOL_RANGE_ASSERT(m_beg <= m_end);
		HOL_DEBUG_ONLY_SECTION
		(
			m_orig_beg = m_beg;
			m_orig_end = m_end;
		)
	}

	explicit basic_range(T* beg, std::size_t len) HOL_RANGE_NOEXCEPT: basic_range(beg, beg + len) {}

	template<std::size_t Size>
	explicit basic_range(T(&a)[Size]) HOL_RANGE_NOEXCEPT: basic_range(a, a + Size) {}

	template<typename Container>
	explicit basic_range(Container&& c) HOL_RANGE_NOEXCEPT
	: basic_range(&std::forward<Container>(c)[0], std::forward<Container>(c).size()) {}

//  &*beg => UB when beg == end
//	template<typename ForwardIter>
//	explicit basic_range(ForwardIter beg, ForwardIter end) HOL_RANGE_NOEXCEPT: basic_range(&*beg, std::distance(beg, end)) {}

	operator basic_range<T const>() const { return basic_range<T const>(*this); }

	constexpr iterator begin() HOL_RANGE_NOEXCEPT { return m_beg; }
	constexpr const_iterator begin() const HOL_RANGE_NOEXCEPT { return m_beg; }

	constexpr iterator end() HOL_RANGE_NOEXCEPT { return m_end; }
	constexpr const_iterator end() const HOL_RANGE_NOEXCEPT { return m_end; }

	constexpr const_iterator cbegin() const HOL_RANGE_NOEXCEPT { return m_beg; }
	constexpr const_iterator cend() const HOL_RANGE_NOEXCEPT { return m_end; }

	constexpr reverse_iterator rbegin() HOL_RANGE_NOEXCEPT { return reverse_iterator{m_end}; }
	constexpr const_reverse_iterator rbegin() const HOL_RANGE_NOEXCEPT { return const_reverse_iterator{m_end}; }

	constexpr reverse_iterator rend() HOL_RANGE_NOEXCEPT { return reverse_iterator{m_beg}; }
	constexpr const_reverse_iterator rend() const HOL_RANGE_NOEXCEPT { return const_reverse_iterator{m_beg}; }

	constexpr const_reverse_iterator crbegin() const HOL_RANGE_NOEXCEPT { return const_reverse_iterator{m_end}; }
	constexpr const_reverse_iterator crend() const HOL_RANGE_NOEXCEPT { return const_reverse_iterator{m_beg}; }

	pointer data() { return m_beg; }
	const_pointer data() const { return m_beg; }

	reference at(std::size_t n) HOL_RANGE_NOEXCEPT { HOL_RANGE_ASSERT(n < size()); return m_beg[n]; }
	const_reference at(std::size_t n) const HOL_RANGE_NOEXCEPT { HOL_RANGE_ASSERT(n < size()); return m_beg[n]; }

	reference operator[](std::size_t n) HOL_RANGE_NOEXCEPT { HOL_RANGE_ASSERT(n < size()); return m_beg[n]; }
	const_reference operator[](std::size_t n) const HOL_RANGE_NOEXCEPT { HOL_RANGE_ASSERT(n < size()); return m_beg[n]; }

	reference front() { HOL_RANGE_ASSERT(!empty()); return *m_beg; }
	const_reference front() const { HOL_RANGE_ASSERT(!empty()); return *m_beg; }

	reference back() { HOL_RANGE_ASSERT(!empty()); return *(m_end - 1); }
	const_reference back() const { HOL_RANGE_ASSERT(!empty()); return *(m_end - 1); }

	std::size_t size() const HOL_RANGE_NOEXCEPT { return m_end - m_beg; }
	bool empty() const HOL_RANGE_NOEXCEPT { return !size(); }

	basic_range subrange(std::size_t pos, std::size_t len) HOL_RANGE_NOEXCEPT
	{
		HOL_RANGE_ASSERT(pos < size());
		if(pos + len > size())
			return basic_range{m_beg + pos, m_end};
		return basic_range{m_beg + pos, m_beg + pos + len};
	}

	basic_range subrange(std::size_t pos) HOL_RANGE_NOEXCEPT
	{
		HOL_RANGE_ASSERT(pos <= size());
		return basic_range{m_beg + pos, m_end};
	}

	bool operator<(basic_range const& other) const
	{
		return std::lexicographical_compare(
			std::begin(*this), std::end(*this),
				std::begin(other), std::end(other));
	}

	bool operator> (basic_range const& other) const { return  other < *this;  }
	bool operator>=(basic_range const& other) const { return !(this < other); }
	bool operator<=(basic_range const& other) const { return !(this > other); }

	bool operator==(basic_range const& other) const
	{
		return std::equal(
			std::begin(*this), std::end(*this),
				std::begin(other), std::end(other));
	}

	bool operator!=(basic_range const& other) const { return !(*this == other); }

	explicit operator bool() const { return !empty(); }

	std::vector<value_type> vector() const { return {m_beg, m_end}; }

	void swap(basic_range& other) HOL_RANGE_NOEXCEPT
	{
		std::swap(m_beg, other.m_beg);
		std::swap(m_end, other.m_end);
		HOL_DEBUG_ONLY_SECTION
		(
			std::swap(m_orig_beg, other.m_orig_beg);
			std::swap(m_orig_end, other.m_orig_end);
		)
	}

	void resize(std::size_t n)
	{
		HOL_RANGE_ASSERT(m_beg + n <= m_orig_end);
		m_end = m_beg + n;
	}

protected:
	pointer m_beg = nullptr;
	pointer m_end = nullptr;
	HOL_DEBUG_ONLY_SECTION
	(
		pointer m_orig_beg = nullptr;
		pointer m_orig_end = nullptr;
	)
};

template<typename T>
using range = basic_range<T>;

template<typename T>
using const_range = basic_range<T const>;

template<typename T>
using range_iterator = typename basic_range<T>::iterator;

template<typename T>
using const_range_iterator = range_iterator<const T>;

template<typename Container>
auto make_range(Container& c)
	{ return range<typename Container::value_type>(c); }

template<typename Container>
auto make_range(Container const& c)
	{ return range<typename Container::value_type const>(c); }

template<typename T>
auto make_range(T* s, std::size_t n)
	{ return range<T>(s, n); }

template<typename T>
auto make_range(T const* s, std::size_t n)
	{ return range<T const>(s, n); }

template<typename T, std::size_t Size>
auto make_range(T(&a)[Size])
	{ return range<T>(a, Size); }

template<typename T, std::size_t Size>
auto make_range(T const(&a)[Size])
	{ return range<T const>(a, Size); }

template<typename T>
auto make_range(T* beg, T* end)
	{ return range<T>(beg, end); }

template<typename T>
auto make_range(T const* beg, T const* end)
	{ return range<T const>(beg, end); }

// Free functions -------------------------------------------

template<typename T>
T& at(basic_range<T> r, std::size_t n) { return r.at(n); }

template<typename T>
T& front(basic_range<T> r) { return r.front(); }

template<typename T>
T& back(basic_range<T> r) { return r.back(); }

template<typename T>
T* data(basic_range<T> r) { return r.data(); }

template<typename T>
bool empty(basic_range<T> r) { return r.empty(); }

template<typename T>
std::size_t size(basic_range<T> r) { return r.size(); }

template<typename T>
std::size_t length(basic_range<T> r) { return r.size(); }

template<typename T>
void clear(basic_range<T>& r) { r = r.subrange(0, 0); }

//template<typename T>
//int compare(basic_range<T> r1, basic_range<T> r2)
//{
//	auto len = std::min(r1.size(), r2.size());
//	return std::char_traits<T>::compare(r1.data(), r2.data(), len);
//}

template<typename T>
void copy(basic_range<T const> src, basic_range<T> dst, std::size_t pos = 0)
{
	HOL_RANGE_ASSERT(pos <= src.size());

	auto beg = std::begin(src) + pos;
	auto end = std::end(src);
	if(beg + dst.size() < end)
		end = beg + dst.size();

	std::copy(beg, end, std::begin(dst));
}

template<typename T>
void copy(basic_range<T> src, basic_range<T> dst, std::size_t pos = 0)
{
	HOL_RANGE_ASSERT(pos <= src.size());
	copy(basic_range<T const>(src), dst, pos);
}

// TODO: find somewhere for this

template<typename CharT, typename Traits>
class basic_range_istream;

template<typename CharT, typename Traits = std::char_traits<CharT>>
class basic_range_buf
: public std::basic_streambuf<CharT, Traits>
{
	friend class basic_range_istream<CharT, Traits>;

public:
	using stream_buf = std::basic_streambuf<CharT, Traits>;

	using int_type = typename stream_buf::int_type;

	basic_range_buf(basic_range<CharT> r): r(r)
		{ stream_buf::setg(r.data(), r.data(), r.data() + r.size()); }

protected:
	using stream_buf::eback;
	using stream_buf::gptr;
	using stream_buf::egptr;
	using stream_buf::gbump;

	std::streamsize showmanyc() override { return egptr() - gptr(); }

	int_type underflow() override
	{
		if(gptr() < egptr())
			return *gptr();
		return EOF;
	}

private:
	basic_range<CharT> _getline(CharT delim)
	{
		auto pos = std::find(gptr(), egptr(), delim);
		basic_range<CharT> line(gptr(), pos);
		if(pos < egptr())
			++pos; // skip delim

		gbump(int(pos - gptr()));
		return line;
	}

	basic_range<CharT> r;
};

template<typename CharT, typename Traits = std::char_traits<CharT>>
basic_range_istream<CharT, Traits>&
getline(basic_range_istream<CharT, Traits>& is, basic_range<CharT>& sp, CharT delim);

template<typename CharT, typename Traits = std::char_traits<CharT>>
class basic_range_istream
: public std::basic_istream<CharT, Traits>
{
	using char_type = typename std::remove_cv<CharT>::type;
	using const_char_type = char_type const;

	friend basic_range_istream& getline<CharT, Traits>(basic_range_istream& is, basic_range<CharT>& sp, CharT delim);

public:
	basic_range_istream(basic_range<CharT> sp): std::basic_istream<CharT, Traits>(&buf), buf(sp) {}

private:
	using istream = std::basic_istream<CharT, Traits>;

	basic_range<CharT> _getline(CharT delim)
	{
		if(std::basic_istream<CharT, Traits>::eof())
		{
			istream::setstate(istream::failbit);
			return {};
		}
		basic_range<CharT> line = buf._getline(delim);
		if(buf.gptr() == buf.egptr())
			istream::setstate(istream::eofbit);
		return line;
	}

	basic_range_buf<CharT, Traits> buf;
};

using range_istream = basic_range_istream<char>;
using wrange_istream = basic_range_istream<wchar_t>;
using u16range_istream = basic_range_istream<char16_t>;
using u32range_istream = basic_range_istream<char32_t>;

template<typename CharT, typename Traits = std::char_traits<CharT>>
basic_range_istream<CharT, Traits>&
getline(basic_range_istream<CharT, Traits>& is, basic_range<CharT>& sp, CharT delim)
{
	if(is)
		sp = is._getline(delim);
	return is;
}

range_istream&
getline(range_istream& is, basic_range<char>& r, char delim = '\n')
	{ return getline<char>(is, r, delim); }

wrange_istream&
getline(wrange_istream& is, basic_range<wchar_t>& r, wchar_t delim = L'\n')
	{ return getline<wchar_t>(is, r, delim); }

u16range_istream&
getline(u16range_istream& is, basic_range<char16_t>& r, char16_t delim = u'\n')
	{ return getline<char16_t>(is, r, delim); }

u32range_istream&
getline(u32range_istream& is, basic_range<char32_t>& r, char32_t delim = U'\n')
	{ return getline<char32_t>(is, r, delim); }

} // namespace range
} // namespace header_only_library

#endif // HOL_RANGE_H
