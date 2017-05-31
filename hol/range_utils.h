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

namespace header_only_library {
namespace range_utils {
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

} // namespace detail

template<typename T>
class range
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

	range() noexcept = default;

	explicit range(T* beg, T* end) noexcept: m_beg(beg), m_end(end) { assert(m_beg <= m_end); }
	explicit range(T* beg, std::size_t len) noexcept: m_beg(beg), m_end(beg + len) { assert(m_beg <= m_end); }

	template<std::size_t Size>
	explicit range(T(&a)[Size]) noexcept
	: m_beg(&*std::begin(a))
	, m_end(&*std::end(a)) {}

	template<typename Container>
	explicit range(Container&& c) noexcept
	: m_beg(&*std::begin(std::forward<Container>(c)))
	, m_end(&*std::end(std::forward<Container>(c))) {}

	template<typename ForwardIter>
	explicit range(ForwardIter beg, ForwardIter end) noexcept
	: m_beg(&*beg)
	, m_end(&*end) {}

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

	reference operator[](std::size_t n) noexcept { assert(n < size()); return m_beg[n]; }
	const_reference operator[](std::size_t n) const noexcept { assert(n < size()); return m_beg[n]; }

	std::size_t size() const noexcept { return m_end - m_beg; }
	bool empty() const noexcept { return !size(); }

	range subrange(std::size_t pos, std::size_t len) noexcept
	{
		assert(pos < size());
		if(pos + len > size())
			return {m_beg + pos, m_end};
		return range{m_beg + pos, m_beg + pos + len};
	}

	range subrange(std::size_t pos) noexcept
	{
		assert(pos < size());
		return range{m_beg + pos, m_end};
	}

	bool operator<(range const& other) const
	{
		return std::lexicographical_compare(
			std::begin(*this), std::end(*this),
				std::begin(other), std::end(other));
	}

	bool operator> (range const& other) const { return  other < *this;  }
	bool operator>=(range const& other) const { return !(this < other); }
	bool operator<=(range const& other) const { return !(this > other); }

	bool operator==(range const& other) const
	{
		return std::equal(
			std::begin(*this), std::end(*this),
				std::begin(other), std::end(other));
	}

	bool operator!=(range const& other) const { return !(*this == other); }

	explicit operator bool() const { return !empty(); }

	std::vector<value_type> vector() const { return {m_beg, m_end}; }

	void swap(range& other) noexcept
		{ std::swap(m_beg, other.m_beg); std::swap(m_end, other.m_end); }

private:
	pointer m_beg = nullptr;
	pointer m_end = nullptr;
};

template<typename Container>
auto make_range(Container& c)
{
	return range<typename Container::value_type>(c);
}

template<typename T, std::size_t Size>
auto make_range(T(&a)[Size])
{
	return range<T>(a);
}

template<typename T, typename UnaryPredicate>
bool all_of(range<T> r, UnaryPredicate p)
	{ return std::all_of(std::begin(r), std::end(r), p); }

template<typename T, typename UnaryPredicate>
bool any_of(range<T> r, UnaryPredicate p)
	{ return std::any_of(std::begin(r), std::end(r), p); }

template<typename T, typename UnaryPredicate>
bool none_of(range<T> r, UnaryPredicate p)
	{ return std::none_of(std::begin(r), std::end(r), p); }

template<typename T, typename UnaryFunction>
UnaryFunction for_each(range<T> r, UnaryFunction f)
	{ return std::for_each(std::begin(r), std::end(r), f); }

template<typename T>
auto count(range<T> r, const T& value)
	{ return std::count(std::begin(r), std::end(r), value); }

template<typename T,typename UnaryPredicate>
auto count(range<T> r, UnaryPredicate p)
	{ return std::count(std::begin(r), std::end(r), p); }

template<typename T>
std::pair<typename range<T>::iterator, typename range<T>::iterator>
mismatch(range<T> r, typename range<T>::iterator i)
{
	return std::mismatch(std::begin(r), std::end(r), i);
}

template<typename T, typename BinaryPredicate>
std::pair<typename range<T>::iterator, typename range<T>::iterator>
mismatch(range<T> r, typename range<T>::iterator i, BinaryPredicate p)
{
	return std::mismatch(std::begin(r), std::end(r), p);
}

template<typename T>
std::pair<typename range<T>::iterator, typename range<T>::iterator>
mismatch(range<T> r1, range<T> r2)
{
	return std::mismatch(std::begin(r1), std::end(r1), std::begin(r2), std::end(r2));
}

template<typename T, typename BinaryPredicate>
std::pair<typename range<T>::iterator, typename range<T>::iterator>
mismatch(range<T> r1, range<T> r2, BinaryPredicate p)
{
	return std::mismatch(std::begin(r1), std::end(r1), std::begin(r2), std::end(r2), p);
}

template<typename T>
bool equal(range<T> r, typename range<T>::iterator i)
	{ return std::equal(std::begin(r), std::end(r), i); }

template<typename T, typename BinaryPredicate>
bool equal(range<T> r, typename range<T>::iterator i, BinaryPredicate p)
	{ return std::equal(std::begin(r), std::end(r), i, p); }

template<typename T>
bool equal(range<T> r1, range<T> r2)
	{ return std::equal(std::begin(r1), std::end(r1), std::begin(r2), std::end(r2)); }

template<typename T, typename BinaryPredicate>
bool equal(range<T> r1, range<T> r2, BinaryPredicate p)
	{ return std::equal(std::begin(r1), std::end(r1), std::begin(r2), std::end(r2), p); }




template<typename T>
std::pair<range<T>, range<T>> find(range<T> r, const T& value)
{
	auto found = std::find(std::begin(r), std::end(r), value);
	return std::make_pair(range<T>(std::begin(r), found), range<T>(found, std::end(r)));
}

template<typename T>
range<T> find_to(range<T> r, const T& value)
{
	auto found = std::find(std::begin(r), std::end(r), value);
	return range<T>(std::begin(r), found);
}

template<typename T>
range<T> find_from(range<T> r, const T& value)
{
	auto found = std::find(std::begin(r), std::end(r), value);
	return range<T>(found, std::end(r));
}

//template<typename ExecutionPolicy, typename ForwardIt, typename T>
//ForwardIt find( ExecutionPolicy&& policy, ForwardIt first, ForwardIt last, const T& value );
//	(2) 	(since C++17)
//template<typename InputIt, typename UnaryPredicate>
//
//InputIt find_if( InputIt first, InputIt last,
//                 UnaryPredicate p );
//	(3)
//template<typename ExecutionPolicy, typename ForwardIt, typename UnaryPredicate>
//
//ForwardIt find_if( ExecutionPolicy&& policy, ForwardIt first, ForwardIt last,
//                 UnaryPredicate p );
//	(4) 	(since C++17)
//template<typename InputIt, typename UnaryPredicate>
//
//InputIt find_if_not( InputIt first, InputIt last,
//                     UnaryPredicate q );
//	(5) 	(since C++11)
//template<typename ExecutionPolicy, typename ForwardIt, typename UnaryPredicate>
//
//ForwardIt find_if_not( ExecutionPolicy&& policy, ForwardIt first, ForwardIt last,
//                     UnaryPredicate q );
//



template<typename T>
void sort(range<T> r) { std::sort(std::begin(r), std::end(r)); }

template<typename T, typename Comp>
void sort(range<T> r, Comp comp) { std::sort(std::begin(r), std::end(r), comp); }

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
std::vector<range<typename Container::value_type>> divide_up_work(Container& v, std::size_t n)
{
	if(!n)
		return {};

	if(n == 1)
		return {range<typename Container::value_type>(v)};

	auto size = v.size();
	auto batch_size = size / n;
	auto extra = size % n;

	std::vector<std::size_t> parts(n, batch_size + (extra / size));

	for(std::size_t i = 0; i < extra % size; ++i)
		++parts[i];

	std::vector<range<typename Container::value_type>> ranges;
	ranges.reserve(n);

	ranges.emplace_back(std::begin(v), parts[0]);
	for(std::size_t i = 1; i < n; ++i)
		ranges.emplace_back(std::end(ranges.back()), std::end(ranges.back()) + parts[i]);
	ranges.emplace_back(std::end(ranges.back()), std::end(v));

	return ranges;
}

// SETS

// STRINGS

using srange = range<char>;
using const_srange = range<char const>;

using wsrange = range<wchar_t>;
using const_wsrange = range<wchar_t const>;

using u16srange = range<char16_t>;
using const_u16srange = range<char16_t const>;

using u32srange = range<char32_t>;
using const_u32srange = range<char32_t const>;

using rmatch = std::match_results<srange::iterator>;
using wrmatch = std::match_results<wsrange::iterator>;
using u16rmatch = std::match_results<u16srange::iterator>;
using u32rmatch = std::match_results<u32srange::iterator>;

std::vector<hol::srange> regex_match(hol::srange s, std::regex const& e,
	std::regex_constants::match_flag_type flags = std::regex_constants::match_default)
{
	std::vector<hol::srange> matches;

	std::match_results<hol::srange::iterator> m;
	if(std::regex_match(std::begin(s), std::end(s), m, e, flags))
		for(auto& sm: m)
			matches.emplace_back(sm.first, sm.second);

	return matches;
}

std::vector<hol::srange> regex_search(hol::srange s, std::regex const& e,
	std::regex_constants::match_flag_type flags = std::regex_constants::match_default)
{
	std::vector<hol::srange> matches;

	std::match_results<hol::srange::iterator> m;
	if(std::regex_search(std::begin(s), std::end(s), m, e, flags))
		for(auto& sm: m)
			matches.emplace_back(sm.first, sm.second);

	return matches;
}

} // namespace range_utils
} // namespace header_only_library

#endif // HOL_RANGE_UTILS_H
