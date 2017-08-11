#ifndef HOL_RANGE_UTILS_H
#define HOL_RANGE_UTILS_H
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
#include <iterator>
#include <regex>
#include <utility>
#include <vector>

#include <cassert>

#ifndef HOL_CONCEPT
#ifdef __cpp_concepts
#define HOL_CONCEPT(c) c
#else
#define HOL_CONCEPT(c)
#endif
#endif // HOL_CONCEPT

#ifndef HOL_DEBUG_ONLY_SECTION
#ifndef NDEBUG
#define HOL_DEBUG_ONLY_SECTION(code) code
#else
#define HOL_DEBUG_ONLY_SECTION(code)
#endif
#endif // HOL_DEBUG_ONLY_SECTION

namespace header_only_library {
namespace range_utils {

namespace concepts {
#ifdef __cpp_concepts

template<typename Char>
concept bool StdCharType
	 = std::is_same<Char, char>::value
	|| std::is_same<Char, wchar_t>::value
	|| std::is_same<Char, char16_t>::value
	|| std::is_same<Char, char32_t>::value;

#endif // __cpp_concept
} // namespace concepts

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

	basic_range() noexcept = default;

	basic_range(basic_range const& r) noexcept = default;
	basic_range(basic_range&& r) noexcept = default;

	basic_range& operator=(basic_range const& r) noexcept = default;
	basic_range& operator=(basic_range&& r) noexcept = default;

	explicit basic_range(T* beg, T* end) noexcept: m_beg(beg), m_end(end)
	{
		assert(m_beg <= m_end);
		HOL_DEBUG_ONLY_SECTION
		(
			m_orig_beg = m_beg;
			m_orig_end = m_end;
		)
	}

	explicit basic_range(T* beg, std::size_t len) noexcept: basic_range(beg, beg + len) {}

	template<std::size_t Size>
	explicit basic_range(T(&a)[Size]) noexcept: basic_range(a, a + Size) {}

	template<typename Container>
	explicit basic_range(Container&& c) noexcept
	: basic_range(&std::forward<Container>(c)[0], std::forward<Container>(c).size()) {}

	template<typename ForwardIter>
	explicit basic_range(ForwardIter beg, ForwardIter end) noexcept: basic_range(&*beg, std::distance(beg, end)) {}

	constexpr iterator begin() noexcept { return m_beg; }
	constexpr const_iterator begin() const noexcept { return m_beg; }

	constexpr iterator end() noexcept { return m_end; }
	constexpr const_iterator end() const noexcept { return m_end; }

	constexpr const_iterator cbegin() const noexcept { return m_beg; }
	constexpr const_iterator cend() const noexcept { return m_end; }

	constexpr reverse_iterator rbegin() noexcept { return reverse_iterator{m_end}; }
	constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator{m_end}; }

	constexpr reverse_iterator rend() noexcept { return reverse_iterator{m_beg}; }
	constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator{m_beg}; }

	constexpr const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator{m_end}; }
	constexpr const_reverse_iterator crend() const noexcept { return const_reverse_iterator{m_beg}; }

	pointer data() { return m_beg; }
	const_pointer data() const { return m_beg; }

	reference at(std::size_t n) noexcept { assert(n < size()); return m_beg[n]; }
	const_reference at(std::size_t n) const noexcept { assert(n < size()); return m_beg[n]; }

	reference operator[](std::size_t n) noexcept { assert(n < size()); return m_beg[n]; }
	const_reference operator[](std::size_t n) const noexcept { assert(n < size()); return m_beg[n]; }

	reference front() { assert(!empty()); return *m_beg; }
	const_reference front() const { assert(!empty()); return *m_beg; }

	reference back() { assert(!empty()); return *(m_end - 1); }
	const_reference back() const { assert(!empty()); return *(m_end - 1); }

	std::size_t size() const noexcept { return m_end - m_beg; }
	bool empty() const noexcept { return !size(); }

	basic_range subrange(std::size_t pos, std::size_t len) noexcept
	{
		assert(pos < size());
		if(pos + len > size())
			return basic_range{m_beg + pos, m_end};
		return basic_range{m_beg + pos, m_beg + pos + len};
	}

	basic_range subrange(std::size_t pos) noexcept
	{
		assert(pos <= size());
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

	void swap(basic_range& other) noexcept
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
		assert(m_beg + n <= m_orig_end);
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

template<typename CharT>
class basic_srange
: public basic_range<CharT>
{
public:
	basic_srange() noexcept: basic_range<CharT>() {}

//	basic_srange(basic_srange const& r) noexcept: basic_range<CharT>(r.data(), r.size()) {}
	basic_srange(basic_srange const& r) noexcept = default;
	basic_srange(basic_srange&& r) noexcept = default;

	basic_srange& operator=(basic_srange const& r) noexcept = default;
	basic_srange& operator=(basic_srange&& r) noexcept = default;

	explicit basic_srange(CharT* beg, CharT* end) noexcept: basic_range<CharT>(beg, end) {}

	explicit basic_srange(CharT* beg, std::size_t len) noexcept: basic_srange(beg, beg + len) {}

	explicit basic_srange(basic_range<CharT> r): basic_srange(r.data(), r.size()) {}


	// This constructor removes the end character (null terminator)
	template<std::size_t Size>
	explicit basic_srange(CharT(&a)[Size]) noexcept: basic_srange(a, a + Size - 1) {}

	template<typename Container>
	explicit basic_srange(Container&& c) noexcept
	: basic_srange(&std::forward<Container>(c)[0], std::forward<Container>(c).size()) {}

	template<typename ForwardIter>
	explicit basic_srange(ForwardIter beg, ForwardIter end) noexcept: basic_srange(&*beg, std::distance(beg, end)) {}

	basic_srange(CharT* s) noexcept
	: basic_srange(s, detail::length_of(s)) {}

	basic_srange substr(std::size_t pos, std::size_t len) noexcept
	{
		return basic_srange<CharT>(basic_range<CharT>::subrange(pos, len));
	}

	basic_srange substr(std::size_t pos) noexcept
	{
		return basic_srange<CharT>(basic_range<CharT>::subrange(pos));
	}

	std::basic_string<typename std::remove_const<CharT>::type>
	string()
	{
		return {basic_range<CharT>::m_beg, basic_range<CharT>::m_end};
	}

};

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

template<typename T, std::size_t Size>
auto make_range(T(&a)[Size])
	{ return range<T>(a, Size); }

template<typename T>
auto make_range(T* beg, T* end)
	{ return range<T>(beg, end); }

// algorithms

template<typename Range, typename UnaryPredicate>
bool all_of(Range r, UnaryPredicate p)
	{ return std::all_of(std::begin(r), std::end(r), p); }

template<typename Range, typename UnaryPredicate>
bool any_of(Range r, UnaryPredicate p)
	{ return std::any_of(std::begin(r), std::end(r), p); }

template<typename Range, typename UnaryPredicate>
bool none_of(Range r, UnaryPredicate p)
	{ return std::none_of(std::begin(r), std::end(r), p); }

template<typename Range, typename UnaryFunction>
UnaryFunction for_each(Range r, UnaryFunction f)
	{ return std::for_each(std::begin(r), std::end(r), f); }

template<typename Range>
auto count(Range r, typename Range::value_type const& value)
	{ return std::count(std::begin(r), std::end(r), value); }

template<typename Range,typename UnaryPredicate>
auto count_if(Range r, UnaryPredicate p)
	{ return std::count_if(std::begin(r), std::end(r), p); }

template<typename Range>
std::pair<typename Range::iterator, typename Range::iterator>
mismatch(Range r, typename Range::iterator i)
	{ return std::mismatch(std::begin(r), std::end(r), i); }

template<typename Range, typename BinaryPredicate>
std::pair<typename Range::iterator, typename Range::iterator>
mismatch(Range r, typename Range::iterator i, BinaryPredicate p)
	{ return std::mismatch(std::begin(r), std::end(r), i, p); }

template<typename Range>
std::pair<typename Range::iterator, typename Range::iterator>
mismatch(Range r1, Range r2)
	{ return std::mismatch(std::begin(r1), std::end(r1), std::begin(r2), std::end(r2)); }

template<typename Range, typename BinaryPredicate>
std::pair<typename Range::iterator, typename Range::iterator>
mismatch(Range r1, Range r2, BinaryPredicate p)
	{ return std::mismatch(std::begin(r1), std::end(r1), std::begin(r2), std::end(r2), p); }

template<typename Range>
bool equal(Range r, typename Range::iterator i)
	{ return std::equal(std::begin(r), std::end(r), i); }

template<typename Range, typename BinaryPredicate>
bool equal(Range r, typename Range::iterator i, BinaryPredicate p)
	{ return std::equal(std::begin(r), std::end(r), i, p); }

template<typename Range>
bool equal(Range r1, Range r2)
	{ return std::equal(std::begin(r1), std::end(r1), std::begin(r2), std::end(r2)); }

template<typename Range, typename BinaryPredicate>
bool equal(Range r1, Range r2, BinaryPredicate p)
	{ return std::equal(std::begin(r1), std::end(r1), std::begin(r2), std::end(r2), p); }

template<typename Range>
typename Range::iterator find(Range r, typename Range::value_type const& value)
	{ return std::find(std::begin(r), std::end(r), value); }

template<typename Range, typename UnaryPredicate>
typename Range::iterator find_if(Range r, UnaryPredicate p)
	{ return std::find_if(std::begin(r), std::end(r), p); }

template<typename Range, typename UnaryPredicate>
typename Range::iterator find_if_not(Range r, UnaryPredicate p)
	{ return std::find_if_not(std::begin(r), std::end(r), p); }

template<typename Range>
typename Range::iterator find_end(Range r1, Range r2)
	{ return std::find_end(std::begin(r1), std::end(r1), std::begin(r2), std::end(r2)); }

template<typename Range, typename BinaryPredicate>
typename Range::iterator find_end(Range r1, Range r2, BinaryPredicate p)
	{ return std::find_end(std::begin(r1), std::end(r1), std::begin(r2), std::end(r2), p); }

template<typename Range>
typename Range::iterator find_first_of(Range r1, Range r2)
	{ return std::find_first_of(std::begin(r1), std::end(r1), std::begin(r2), std::end(r2)); }

template<typename Range, typename BinaryPredicate>
typename Range::iterator find_first_of(Range r1, Range r2, BinaryPredicate p)
	{ return std::find_first_of(std::begin(r1), std::end(r1), std::begin(r2), std::end(r2), p); }

template<typename Range, typename BinaryPredicate>
typename Range::iterator adjacent_find(Range r, BinaryPredicate p)
	{ return std::adjacent_find(std::begin(r), std::end(r), p); }

template<typename Range>
typename Range::iterator search(Range r1, Range r2)
	{ return std::search(std::begin(r1), std::end(r1), std::begin(r2), std::end(r2)); }

template<typename Range, typename BinaryPredicate>
typename Range::iterator search(Range r1, Range r2, BinaryPredicate p)
	{ return std::search(std::begin(r1), std::end(r1), std::begin(r2), std::end(r2), p); }

template<typename Size, typename Range>
typename Range::iterator search_n(Range r, Size count, typename Range::value_type const& value)
	{ return std::search_n(std::begin(r), std::end(r), count, value); }

template<typename Size, typename Range, typename BinaryPredicate>
typename Range::iterator search_n(Range r, Size count, typename Range::value_type const& value, BinaryPredicate p)
	{ return std::search_n(std::begin(r), std::end(r), count, value, p); }



template<typename Range, typename OutIter>
OutIter copy(Range r, OutIter d)
	{ return std::copy(std::begin(r), std::end(r), d); }

template<typename Range, typename OutIter, typename UnaryPredicate>
OutIter copy_if(Range r, OutIter d, UnaryPredicate p)
	{ return std::copy_if(std::begin(r), std::end(r), d, p); }


// SPLICE ? splice_to() splice_from()?
namespace detail {

template<typename Range>
std::pair<Range, Range> splice_at(Range r, typename Range::iterator i)
	{ return std::make_pair(Range(std::begin(r), i), Range(i, std::end(r))); }

template<typename Range>
Range splice_to(Range r, typename Range::iterator i)
	{ return Range(std::begin(r), i); }

template<typename Range>
Range splice_from(Range r, typename Range::iterator i)
	{ return Range(i, std::end(r)); }

} // namespace detail

template<typename Range>
std::pair<Range, Range> splice_at(Range r, typename Range::value_type const& value)
	{ return detail::splice_at(r, std::find(std::begin(r), std::end(r), value)); }

template<typename Range, typename UnaryPredicate>
std::pair<Range, Range> splice_at(Range r, UnaryPredicate p)
	{ return detail::splice_at(r, std::find(std::begin(r), std::end(r), p)); }

template<typename Range>
Range splice_to(Range r, typename Range::value_type const& value)
	{ return detail::splice_to(r, std::find(std::begin(r), std::end(r), value)); }

template<typename Range, typename UnaryPredicate>
Range splice_to(Range r, UnaryPredicate p)
	{ return detail::splice_to(r, std::find(std::begin(r), std::end(r), p)); }

template<typename Range>
Range splice_from(Range r, typename Range::value_type const& value)
	{ return detail::splice_from(r, std::find(std::begin(r), std::end(r), value)); }

template<typename Range, typename UnaryPredicate>
Range splice_from(Range r, UnaryPredicate p)
	{ return detail::splice_from(r, std::find(std::begin(r), std::end(r), p)); }



template<typename Range>
void sort(Range r) { std::sort(std::begin(r), std::end(r)); }

template<typename Range, typename Compare>
void sort(Range r, Compare comp) { std::sort(std::begin(r), std::end(r), comp); }


template<typename Range>
Range equal_range(Range r, typename Range::value_type const& value)
{
	assert(std::is_sorted(std::begin(r), std::end(r)));
	auto er = std::equal_range(std::begin(r), std::end(r), value);
	return Range{er.first, er.second};
}

template<typename Range, typename Compare>
Range equal_range(Range r, Compare comp)
{
	assert(std::is_sorted(std::begin(r), std::end(r)));
	auto er = std::equal_range(std::begin(r), std::end(r), comp);
	return Range{er.first, er.second};
}


// Other Utils

// for dividing a container into multiple threads
//template<typename T>
//std::vector<range<T>> divide_up_work(std::vector<T>& v, std::size_t n)
//{
//	bug_fun();
//
//	assert(n);
//
//	std::vector<typename std::vector<T>::iterator> iters;
//	iters.reserve(n + 1);
//
//	auto const batch_size = v.size() / n;
//
//	iters.emplace_back(std::begin(v));
//	while(--n)
//		iters.emplace_back(iters.back() + batch_size);
//	iters.emplace_back(std::end(v));
//
//	bug_var(iters.size());
//
//	std::vector<range<T>> ranges;
//	ranges.reserve(n);
//
//	for(auto iter = std::next(begin(iters)); iter != std::end(iters); ++iter)
//		ranges.emplace_back(*std::prev(iter), *iter);
//
//	return ranges;
//}

// for dividing a container into multiple threads
// divides the work up equally giving the  lower indexes distribution priority
template<typename Container>
std::vector<basic_range<typename Container::value_type>> divide_up_work(Container& v, std::size_t n)
{
	if(!n)
		return {};

	if(n == 1)
		return {basic_range<typename Container::value_type>(v)};

	auto size = v.size();
	auto batch_size = size / n;
	auto extra = size % n;

	std::vector<std::size_t> parts(n, batch_size + (extra / size));

	for(std::size_t i = 0; i < extra % size; ++i)
		++parts[i];

	std::vector<basic_range<typename Container::value_type>> ranges;
	ranges.reserve(n);

	ranges.emplace_back(std::begin(v), parts[0]);
	for(std::size_t i = 1; i < n; ++i)
		ranges.emplace_back(std::end(ranges.back()), std::end(ranges.back()) + parts[i]);
	ranges.emplace_back(std::end(ranges.back()), std::end(v));

	return ranges;
}

namespace detail {
inline
std::size_t calc_part(std::size_t rem, std::size_t div, std::size_t part_id)
{
	if(part_id < rem)
		return (div * part_id) + part_id;

	return (div * part_id) + rem;
}
} // namespace detail

template<typename T>
std::vector<range<T>> divide_up_work(range<T> r, std::size_t n)
{
	std::size_t rem = r.size() % n;
	std::size_t div = r.size() / n;

	std::vector<range<T>> ranges;
	ranges.reserve(n);

	for(std::size_t i = 0; i < n; ++i)
	{
		std::size_t beg = detail::calc_part(rem, div, i);
		std::size_t end = detail::calc_part(rem, div, i + 1);

		ranges.emplace_back(r.data() + beg, r.data() + end);
	}

	return ranges;
}

// stuff

template<typename Range>
bool is_sorted(Range r)
	{ return std::is_sorted(std::begin(r), std::end(r)); }

template<typename Range,typename Compare>
bool is_sorted(Range r, Compare comp)
	{ return std::is_sorted(std::begin(r), std::end(r), comp); }

template<typename Range>
typename Range::iterator is_sorted_until(Range r)
	{ return std::is_sorted_until(std::begin(r), std::end(r)); }

template<typename Range,typename Compare>
typename Range::iterator is_sorted_until(Range r, Compare comp)
	{ return std::is_sorted_until(std::begin(r), std::end(r), comp); }

// SETS

/**
 * Return the leftmost range of two, potentially
 * intersecting ranges. If all of r1 < r2 then r1
 * is returned and vice versa. If the ranges intersect
 * then the leftmost range after each range's borders
 * "cut through" the intersecting range.
 *
 * The results are only meaningful of both ranges belong
 * to the same array.
 *
 * @param r1 The first of two ranges that may intersect.
 * @param r2 The second of two ranges that may intersect.
 *
 * @return The leftmost range after splicing.
 */
template<typename Range>
Range left_splice(Range r1, Range r2)
{
	if(r1.data() < r2.data())
		return Range{r1.data(), std::min(r1.data() + r1.size(), r2.data())};
	return Range{r2.data(), std::min(r2.data() + r2.size(), r1.data())};
}

/**
 * Return the rightmost range of two, potentially
 * intersecting ranges. If all of r1 > r2 then r1
 * is returned and vice versa. If the ranges intersect
 * then the rightmost range after each range's borders
 * "cut through" the intersecting range.
 *
 * The results are only meaningful of both ranges belong
 * to the same array.
 *
 * @param r1 The first of two ranges that may intersect.
 * @param r2 The second of two ranges that may intersect.
 *
 * @return The rightmost range after splicing.
 */
template<typename Range>
Range right_splice(Range r1, Range r2)
{
	if(r1.data() + r1.size() < r2.data() + r2.size())
		return Range{std::max(r1.data() + r1.size(), r2.data()), r2.data() + r2.size()};
	return Range{std::max(r2.data() + r2.size(), r1.data()), r1.data() + r1.size()};
}

// STRINGS

//template<>
//class range<char>
//{
//
//};

using srange = basic_srange<char>;
using const_srange = basic_srange<char const>;

using wsrange = basic_srange<wchar_t>;
using const_wsrange = basic_srange<wchar_t const>;

using u16srange = basic_srange<char16_t>;
using const_u16srange = basic_srange<char16_t const>;

using u32srange = basic_srange<char32_t>;
using const_u32srange = basic_srange<char32_t const>;

template<typename String>
auto make_srange(String& c)
	{ return basic_srange<typename String::value_type>(c); }

template<typename String>
auto make_srange(String const& c)
	{ return basic_srange<typename String::value_type const>(c); }

template<typename CharT>
auto make_srange(CharT* s, std::size_t n)
	{ return basic_srange<CharT>(s, n); }

template<typename CharT, std::size_t Size>
auto make_srange(CharT(&a)[Size])
	{ return basic_srange<CharT>(a, Size); }

template<typename CharT>
auto make_srange(CharT* beg, CharT* end)
	{ return basic_srange<CharT>(beg, end); }

using rmatch = std::match_results<srange::iterator>;
using wrmatch = std::match_results<wsrange::iterator>;
using u16rmatch = std::match_results<u16srange::iterator>;
using u32rmatch = std::match_results<u32srange::iterator>;

template<typename BiIter, typename Char = typename std::iterator_traits<BiIter>::value_type>
using basic_rregex_iterator = std::regex_iterator<BiIter, Char>;

template<typename BiIter, typename Char = typename std::iterator_traits<BiIter>::value_type>
using basic_rregex_token_iterator = std::regex_token_iterator<BiIter, Char>;

using rregex_iterator = basic_rregex_iterator<srange::iterator>;
using rregex_token_iterator = basic_rregex_token_iterator<srange::iterator>;

namespace detail {

template<typename MatchType>
std::vector<basic_range<typename MatchType::char_type>> convert_matches(MatchType const& m)
{
	std::vector<basic_range<typename MatchType::char_type>> matches;
	matches.reserve(m.size());

	for(auto& sm: m)
		matches.emplace_back(sm.first, sm.second);

	return matches;
}

} // namespace detail

template<typename Char>
std::vector<basic_range<Char>> regex_match(basic_range<Char> s, std::regex const& e,
	std::regex_constants::match_flag_type flags = std::regex_constants::match_default)
{
	std::match_results<typename basic_range<Char>::iterator> m;
	if(!std::regex_match(std::begin(s), std::end(s), m, e, flags))
		return {};

	return detail::convert_matches(m);

//	std::vector<range<Char>> matches;
//	matches.reserve(m.size());
//
//	for(auto& sm: m)
//		matches.emplace_back(sm.first, sm.second);
//
//	return matches;
}

template<typename Char>
std::vector<basic_range<Char>> regex_search(basic_range<Char> s, std::regex const& e,
	std::regex_constants::match_flag_type flags = std::regex_constants::match_default)
{

	std::match_results<typename basic_range<Char>::iterator> m;
	if(!std::regex_search(std::begin(s), std::end(s), m, e, flags))
		return {};

	return detail::convert_matches(m);

//	std::vector<range<Char>> matches;
//	matches.reserve(m.size());
//
//	for(auto& sm: m)
//		matches.emplace_back(sm.first, sm.second);
//
//	return matches;
}

template<typename Char>
std::vector<std::vector<basic_range<Char>>> regex_search_all(basic_range<Char> s, std::regex const& e,
	std::regex_constants::match_flag_type flags = std::regex_constants::match_default)
{

	std::match_results<typename basic_range<Char>::iterator> m;
	if(!std::regex_search(std::begin(s), std::end(s), m, e, flags))
		return {};

	std::vector<std::vector<basic_range<Char>>> matches;

	basic_rregex_iterator<typename basic_range<Char>::iterator> end;
	basic_rregex_iterator<typename basic_range<Char>::iterator> itr(std::begin(s), std::end(s), e, flags);

	for(; itr != end; ++itr)
		matches.emplace_back(detail::convert_matches(*itr));

	return matches;
}

// trimming

namespace detail {

constexpr auto const& ws(char) { return " \t\n\r\f\v\0"; }
constexpr auto const& ws(wchar_t) { return L" \t\n\r\f\v\0"; }
constexpr auto const& ws(char16_t) { return u" \t\n\r\f\v\0"; }
constexpr auto const& ws(char32_t) { return U" \t\n\r\f\v\0"; }

//range<char const> wsr(char c) { return range<char const>(std::begin(ws(c)), std::end(ws(c))); }
//range<wchar_t const> wsr(wchar_t c) { return range<wchar_t const>(std::begin(ws(c)), std::end(ws(c))); }
//range<char16_t const> wsr(char16_t c) { return range<char16_t const>(std::begin(ws(c)), std::end(ws(c))); }
//range<char32_t const> wsr(char32_t c) { return range<char32_t const>(std::begin(ws(c)), std::end(ws(c))); }
} // namespace detail

template<typename Char>
basic_range<Char> trim_left(basic_range<Char> r, basic_range<Char> ws = detail::ws(Char()))
{
	auto pos = std::find_if_not(std::begin(r), std::end(r), [&](auto c)
		{ return std::find(std::begin(ws), std::end(ws), c) != std::end(ws); });

	return make_range(pos, std::end(r));
}

template<typename Char>
basic_range<Char> trim_right(basic_range<Char> r, basic_range<Char> ws = detail::ws(Char()))
{
	auto rpos = std::find_if_not(std::rbegin(r), std::rend(r), [&](auto c)
		{ return std::find(std::begin(ws), std::end(ws), c) != std::end(ws); });

	return make_range(std::begin(r), rpos.base());
}

template<typename Char>
basic_range<Char> trim(basic_range<Char> r, basic_range<Char> ws = detail::ws(Char()))
{
	return trim_left(trim_right(r, ws), ws);
}

// string functions (std::string members as free functions)

template<typename Char>
//HOL_CONCEPT(requires concepts::StdCharType<Char>)
Char& at(basic_range<Char> r, std::size_t n) { return r.at(n); }

template<typename Char>
Char& front(basic_range<Char> r) { return r.front(); }

template<typename Char>
Char& back(basic_range<Char> r) { return r.back(); }

template<typename Char>
Char* data(basic_range<Char> r) { return r.data(); }

template<typename Char>
Char* c_str(basic_range<Char> r) { return r.data(); }

template<typename Char>
bool empty(basic_range<Char> r) { return r.empty(); }

template<typename Char>
std::size_t size(basic_range<Char> r) { return r.size(); }

template<typename Char>
std::size_t length(basic_range<Char> r) { return r.size(); }

template<typename Char>
void clear(basic_range<Char>& r) { return r = r.subrange(0, 0); }

template<typename Char>
int compare(basic_range<Char> r1, basic_range<Char> r2)
{
	auto len = std::min(r1.size(), r2.size());
	return std::char_traits<Char>::compare(r1.data(), r2.data(), len);
}

template<typename Char>
void copy(basic_range<Char> r1, basic_range<Char> r2, std::size_t pos = 0)
{
	assert(pos <= r1.size());

	auto beg = std::begin(r1) + pos;
	auto end = std::end(r1);
	if(beg + r2.size() < end)
		end = beg + r2.size();

	std::copy(beg, end, std::begin(r2));
}

int stoi(basic_range<char> r, std::size_t* pos = 0, int base = 10)
{
	auto ptr = r.data();
	auto i = std::strtol(r.data(), &ptr, base);

	if(ptr == r.data())
		throw std::invalid_argument("invalid argument: stoi(" + std::string(std::begin(r), std::end(r)) + ")");

	if(i > std::numeric_limits<int>::max()
	|| i < std::numeric_limits<int>::min())
		throw std::out_of_range("out of range: stoi(" + std::string(std::begin(r), std::end(r)) + ")");

	if(pos)
		*pos = std::size_t(std::distance(r.data(), ptr));

	return int(i);
}

long stol(basic_range<char> r, std::size_t* pos = 0, int base = 10)
{
	auto ptr = r.data();
	auto i = std::strtoll(r.data(), &ptr, base);

	if(ptr == r.data())
		throw std::invalid_argument("invalid argument: stol(" + std::string(std::begin(r), std::end(r)) + ")");

	if(i > std::numeric_limits<long>::max()
	|| i < std::numeric_limits<long>::min())
		throw std::out_of_range("out of range: stol(" + std::string(std::begin(r), std::end(r)) + ")");

	if(pos)
		*pos = std::size_t(std::distance(r.data(), ptr));

	return int(i);
}

long long stoll(basic_range<char> r, std::size_t* pos = 0, int base = 10)
{
	auto ptr = r.data();
	auto i = std::strtoll(r.data(), &ptr, base);

	if(ptr == r.data())
		throw std::invalid_argument("invalid argument: stoll(" + std::string(std::begin(r), std::end(r)) + ")");

	if(errno == ERANGE)
		throw std::out_of_range("out of range: stoll(" + std::string(std::begin(r), std::end(r)) + ")");

	if(pos)
		*pos = std::size_t(std::distance(r.data(), ptr));

	return int(i);
}

// range_istream

//template<typename CharT, typename Traits>
//class basic_range_istream;
//
//template<typename CharT, typename Traits = std::char_traits<CharT>>
//class basic_range_buf
//: public std::basic_streambuf<CharT, Traits>
//{
//	friend class basic_range_istream<CharT, Traits>;
//
//public:
//	using stream_buf = std::basic_streambuf<CharT, Traits>;
//
//	using int_type = typename stream_buf::int_type;
//
//	basic_range_buf(range<CharT> r): r(r)
//		{ stream_buf::setg(r.data(), r.data(), r.data() + r.size()); }
//
//protected:
//	using stream_buf::eback;
//	using stream_buf::gptr;
//	using stream_buf::egptr;
//	using stream_buf::gbump;
//
//	int_type underflow() override
//	{
//		if(gptr() < egptr())
//			return *gptr();
//		return EOF;
//	}
//
//private:
//	range<CharT> _getline(CharT delim)
//	{
//		auto pos = std::find(gptr(), egptr(), delim);
//		range<CharT> line(gptr(), pos);
//		if(pos < egptr())
//			++pos; // skip delim
//
//		gbump(int(pos - gptr()));
//		return line;
//	}
//
//	range<CharT> r;
//};
//
//template<typename CharT, typename Traits = std::char_traits<CharT>>
//basic_range_istream<CharT, Traits>&
//getline(basic_range_istream<CharT, Traits>& is, range<CharT>& sp, CharT delim);
//
//template<typename CharT, typename Traits = std::char_traits<CharT>>
//class basic_range_istream
//: public std::basic_istream<CharT, Traits>
//{
//	using char_type = typename std::remove_cv<CharT>::type;
//	using const_char_type = char_type const;
//
//	friend basic_range_istream& getline<CharT, Traits>(basic_range_istream& is, range<CharT>& sp, CharT delim);
//
//public:
//	basic_range_istream(range<CharT> sp): std::istream(&buf), buf(sp) {}
//
//private:
//	using istream = std::basic_istream<CharT, Traits>;
//
//	range<CharT> _getline(CharT delim)
//	{
//		if(std::basic_istream<CharT, Traits>::eof())
//		{
//			istream::setstate(istream::failbit);
//			return {};
//		}
//		range<CharT> line = buf._getline(delim);
//		if(buf.gptr() == buf.egptr())
//			istream::setstate(istream::eofbit);
//		return line;
//	}
//	basic_range_buf<CharT, Traits> buf;
//};
//
//using range_istream = basic_range_istream<char>;
//using wrange_istream = basic_range_istream<wchar_t>;
//using u16range_istream = basic_range_istream<char16_t>;
//using u32range_istream = basic_range_istream<char32_t>;
//
//template<typename CharT, typename Traits = std::char_traits<CharT>>
//basic_range_istream<CharT, Traits>&
//getline(basic_range_istream<CharT, Traits>& is, range<CharT>& sp, CharT delim)
//{
//	sp = is._getline(delim);
//	return is;
//}
//
//range_istream&
//getline(range_istream& is, range<char>& r, char delim = '\n')
//	{ return getline<char>(is, r, delim); }
//
//wrange_istream&
//getline(wrange_istream& is, range<wchar_t>& r, wchar_t delim = L'\n')
//	{ return getline<wchar_t>(is, r, delim); }
//
//u16range_istream&
//getline(u16range_istream& is, range<char16_t>& r, char16_t delim = u'\n')
//	{ return getline<char16_t>(is, r, delim); }
//
//u32range_istream&
//getline(u32range_istream& is, range<char32_t>& r, char32_t delim = U'\n')
//	{ return getline<char32_t>(is, r, delim); }

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

//using const_range_istream = basic_range_istream<char const>;
//using wrange_istream = basic_range_istream<wchar_t>;
//using u16range_istream = basic_range_istream<char16_t>;
//using u32range_istream = basic_range_istream<char32_t>;

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

namespace literals {

auto operator""_sr(char const* s, std::size_t n) noexcept
	{ return const_srange{s, n}; }

auto operator""_wsr(wchar_t const* s, std::size_t n) noexcept
	{ return const_wsrange{s, n}; }

auto operator""_u16sr(char16_t const* s, std::size_t n) noexcept
	{ return const_u16srange{s, n}; }

auto operator""_u32sr(char32_t const* s, std::size_t n) noexcept
	{ return const_u32srange{s, n}; }

//	using range_istream = basic_range_istream<char>;
//	using wrange_istream = basic_range_istream<wchar_t>;
//	using u16range_istream = basic_range_istream<char16_t>;
//	using u32range_istream = basic_range_istream<char32_t>;

//auto operator""_ris(char const* s, std::size_t n) noexcept
//	{ return range_istream(const_srange(s, n)); }

// Splitting

template<typename RangeType, typename CRangeType>
std::vector<RangeType> split_range(RangeType s, CRangeType t)
{
	std::vector<RangeType> v;

	auto beg = s.data();
	auto const end = s.data() + s.size();
	decltype(beg) pos;

	while((pos = std::search(beg, end, t.data(), t.data() + t.size())) != end)
	{
		v.emplace_back(beg, pos);
		beg = pos + t.size();
	}

	if(!v.empty())
		v.emplace_back(beg, pos);

	return v;
}

} // literals
} // namespace range_utils
} // namespace header_only_library

#endif // HOL_RANGE_UTILS_H
