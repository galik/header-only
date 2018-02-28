#ifndef HEADER_ONLY_LIBRARY_RANGE_UTILS_H
#define HEADER_ONLY_LIBRARY_RANGE_UTILS_H
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
#include <type_traits>
#include <utility>
#include <vector>

#include <cassert>

#include "range.h"

namespace header_only_library {
namespace range {

// algorithms

template<typename T, typename UnaryPredicate>
bool all_of(basic_range<T> r, UnaryPredicate p)
	{ return std::all_of(std::begin(r), std::end(r), p); }

template<typename T, typename UnaryPredicate>
bool any_of(basic_range<T> r, UnaryPredicate p)
	{ return std::any_of(std::begin(r), std::end(r), p); }

template<typename T, typename UnaryPredicate>
bool none_of(basic_range<T> r, UnaryPredicate p)
	{ return std::none_of(std::begin(r), std::end(r), p); }

template<typename T, typename UnaryFunction>
UnaryFunction for_each(basic_range<T> r, UnaryFunction f)
	{ return std::for_each(std::begin(r), std::end(r), f); }

template<typename T>
auto count(basic_range<T> r, T const& value)
	{ return std::count(std::begin(r), std::end(r), value); }

template<typename T, typename UnaryPredicate>
auto count_if(basic_range<T> r, UnaryPredicate p)
	{ return std::count_if(std::begin(r), std::end(r), p); }

template<typename T>
auto mismatch(basic_range<T> r, typename basic_range<T>::iterator i)
	{ return std::mismatch(std::begin(r), std::end(r), i); }

template<typename T, typename BinaryPredicate>
auto mismatch(basic_range<T> r, typename basic_range<T>::iterator i, BinaryPredicate p)
	{ return std::mismatch(std::begin(r), std::end(r), i, p); }

template<typename T>
auto mismatch(basic_range<T> r1, basic_range<T> r2)
	{ return std::mismatch(std::begin(r1), std::end(r1), std::begin(r2), std::end(r2)); }

template<typename T, typename BinaryPredicate>
auto mismatch(basic_range<T> r1, basic_range<T> r2, BinaryPredicate p)
	{ return std::mismatch(std::begin(r1), std::end(r1), std::begin(r2), std::end(r2), p); }

template<typename T>
bool equal(basic_range<T> r, typename basic_range<T>::iterator i)
	{ return std::equal(std::begin(r), std::end(r), i); }

template<typename T, typename BinaryPredicate>
bool equal(basic_range<T> r, typename basic_range<T>::iterator i, BinaryPredicate p)
	{ return std::equal(std::begin(r), std::end(r), i, p); }

template<typename T>
bool equal(basic_range<T> r1, basic_range<T> r2)
	{ return std::equal(std::begin(r1), std::end(r1), std::begin(r2), std::end(r2)); }

template<typename T, typename BinaryPredicate>
bool equal(basic_range<T> r1, basic_range<T> r2, BinaryPredicate p)
	{ return std::equal(std::begin(r1), std::end(r1), std::begin(r2), std::end(r2), p); }

template<typename T>
auto find(basic_range<T> r, T const& value)
		{ return std::find(std::begin(r), std::end(r), value); }

template<typename T, typename UnaryPredicate>
auto find_if(basic_range<T> r, UnaryPredicate p)
	{ return std::find_if(std::begin(r), std::end(r), p); }

template<typename T, typename UnaryPredicate>
auto find_if_not(basic_range<T> r, UnaryPredicate p)
	{ return std::find_if_not(std::begin(r), std::end(r), p); }

template<typename T>
auto find_end(basic_range<T> r1, basic_range<T> r2)
	{ return std::find_end(std::begin(r1), std::end(r1), std::begin(r2), std::end(r2)); }

template<typename T, typename BinaryPredicate>
auto find_end(basic_range<T> r1, basic_range<T> r2, BinaryPredicate p)
	{ return std::find_end(std::begin(r1), std::end(r1), std::begin(r2), std::end(r2), p); }

template<typename T>
auto find_first_of(basic_range<T> r1, basic_range<T> r2)
	{ return std::find_first_of(std::begin(r1), std::end(r1), std::begin(r2), std::end(r2)); }

template<typename T, typename BinaryPredicate>
auto find_first_of(basic_range<T> r1, basic_range<T> r2, BinaryPredicate p)
	{ return std::find_first_of(std::begin(r1), std::end(r1), std::begin(r2), std::end(r2), p); }

template<typename T, typename BinaryPredicate>
auto adjacent_find(basic_range<T> r, BinaryPredicate p)
	{ return std::adjacent_find(std::begin(r), std::end(r), p); }

template<typename T>
decltype(auto) search(basic_range<T> r1, basic_range<T> r2)
	{ return std::search(std::begin(r1), std::end(r1), std::begin(r2), std::end(r2)); }

template<typename T, typename BinaryPredicate>
decltype(auto) search(basic_range<T> r1, basic_range<T> r2, BinaryPredicate p)
	{ return std::search(std::begin(r1), std::end(r1), std::begin(r2), std::end(r2), p); }

template<typename Size, typename T>
decltype(auto) search_n(basic_range<T> r, Size count, T const& value)
	{ return std::search_n(std::begin(r), std::end(r), count, value); }

template<typename Size, typename T, typename BinaryPredicate>
decltype(auto) search_n(basic_range<T> r, Size count, T const& value, BinaryPredicate p)
	{ return std::search_n(std::begin(r), std::end(r), count, value, p); }



template<typename T, typename OutIter>
OutIter copy(basic_range<T> r, OutIter d)
	{ return std::copy(std::begin(r), std::end(r), d); }

template<typename T, typename OutIter, typename UnaryPredicate>
OutIter copy_if(basic_range<T> r, OutIter d, UnaryPredicate p)
	{ return std::copy_if(std::begin(r), std::end(r), d, p); }

// SPLICE ? splice_to() splice_from()?

template<typename T>
auto splice_at(basic_range<T> r, typename basic_range<T>::iterator i)
	{ return std::make_pair(basic_range<T>(std::begin(r), i), basic_range<T>(i, std::end(r))); }

template<typename T>
auto splice_at(basic_range<T> r, T const& value)
	{ return splice_at(r, std::find(std::begin(r), std::end(r), value)); }

template<typename T, typename UnaryPredicate>
auto splice_at(basic_range<T> r, UnaryPredicate p)
	{ return splice_at(r, std::find(std::begin(r), std::end(r), p)); }

template<typename T>
auto splice_to(basic_range<T> r, typename basic_range<T>::iterator i)
	{ return basic_range<T>(std::begin(r), i); }

template<typename T>
auto splice_to(basic_range<T> r, T const& value)
	{ return splice_to(r, std::find(std::begin(r), std::end(r), value)); }

template<typename T, typename UnaryPredicate>
auto splice_to(basic_range<T> r, UnaryPredicate p)
	{ return splice_to(r, std::find(std::begin(r), std::end(r), p)); }

template<typename T>
auto splice_from(basic_range<T> r, typename basic_range<T>::iterator i)
	{ return basic_range<T>(i, std::end(r)); }

template<typename T>
auto splice_from(basic_range<T> r, T const& value)
	{ return splice_from(r, std::find(std::begin(r), std::end(r), value)); }

template<typename T, typename UnaryPredicate>
auto splice_from(basic_range<T> r, UnaryPredicate p)
	{ return splice_from(r, std::find(std::begin(r), std::end(r), p)); }



template<typename T>
void sort(basic_range<T> r) { std::sort(std::begin(r), std::end(r)); }

template<typename T, typename Compare>
void sort(basic_range<T> r, Compare comp) { std::sort(std::begin(r), std::end(r), comp); }


template<typename T>
auto equal_range(basic_range<T> r, T const& value)
{
	assert(std::is_sorted(std::begin(r), std::end(r)));
	auto er = std::equal_range(std::begin(r), std::end(r), value);
	return basic_range<T>{er.first, er.second};
}

template<typename T, typename Compare>
auto equal_range(basic_range<T> r, Compare comp)
{
	assert(std::is_sorted(std::begin(r), std::end(r)));
	auto er = std::equal_range(std::begin(r), std::end(r), comp);
	return basic_range<T>{er.first, er.second};
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

template<typename T>
bool is_sorted(basic_range<T> r)
	{ return std::is_sorted(std::begin(r), std::end(r)); }

template<typename T,typename Compare>
bool is_sorted(basic_range<T> r, Compare comp)
	{ return std::is_sorted(std::begin(r), std::end(r), comp); }

template<typename T>
auto is_sorted_until(basic_range<T> r)
	{ return std::is_sorted_until(std::begin(r), std::end(r)); }

template<typename T,typename Compare>
auto is_sorted_until(basic_range<T> r, Compare comp)
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
template<typename T>
auto left_splice(basic_range<T> r1, basic_range<T> r2)
{
	if(r1.data() < r2.data())
		return basic_range<T>{r1.data(), std::min(r1.data() + r1.size(), r2.data())};
	return basic_range<T>{r2.data(), std::min(r2.data() + r2.size(), r1.data())};
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
template<typename T>
auto right_splice(basic_range<T> r1, basic_range<T> r2)
{
	if(r1.data() + r1.size() < r2.data() + r2.size())
		return basic_range<T>{std::max(r1.data() + r1.size(), r2.data()), r2.data() + r2.size()};
	return basic_range<T>{std::max(r2.data() + r2.size(), r1.data()), r1.data() + r1.size()};
}

} // namespace range
} // namespace header_only_library

#endif // HEADER_ONLY_LIBRARY_RANGE_UTILS_H
