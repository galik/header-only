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

	range(T* beg, T* end) noexcept: m_beg(beg), m_end(end) { assert(m_beg <= m_end); }

	template<std::size_t Size>
	range(T(&a)[Size]) noexcept
	: m_beg(&*std::begin(a))
	, m_end(&*std::end(a)) {}

	template<typename Container>
	range(Container&& c) noexcept
	: m_beg(&*std::begin(std::forward<Container>(c)))
	, m_end(&*std::end(std::forward<Container>(c))) {}

	iterator begin() noexcept { return m_beg; }
	const_iterator begin() const noexcept { return m_beg; }

	iterator end() noexcept { return m_end; }
	const_iterator end() const noexcept { return m_end; }

	const_iterator cbegin() const noexcept { return m_beg; }
	const_iterator cend() const noexcept { return m_end; }

	reverse_iterator rbegin() { return reverse_iterator{m_end}; }
	const_reverse_iterator rbegin() const { return const_reverse_iterator{m_end}; }

	reverse_iterator rend() { return reverse_iterator{m_beg}; }
	const_reverse_iterator rend() const { return const_reverse_iterator{m_beg}; }

	const_reverse_iterator crbegin() const { return const_reverse_iterator{m_end}; }
	const_reverse_iterator crend() const { return const_reverse_iterator{m_beg}; }

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
		return {m_beg + pos, m_beg + pos + len};
	}

	bool operator==(range const& other) const
	{
		return std::lexicographical_compare(
			std::begin(*this), std::end(*this),
				std::begin(other), std::end(other));
	}

	bool operator!=(range const& other) const { return !(*this == other); }

private:
	pointer m_beg = nullptr;
	pointer m_end = nullptr;
};

template<typename T>
void sort(range<T> r) { std::sort(std::begin(r), std::end(r)); }

template<typename T, typename Comp>
void sort(range<T> r, Comp comp) { std::sort(std::begin(r), std::end(r), comp); }

} // namespace range_utils
} // namespace header_only_library

#endif // HOL_RANGE_UTILS_H
