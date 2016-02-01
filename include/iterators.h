#ifndef HOL_ITERATORS_H
#define HOL_ITERATORS_H
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

#include <random>

namespace hol {

template<typename NumericType>
struct IncrementBy
{
	NumericType by;
	constexpr IncrementBy(NumericType by = 1): by(by) {}
	NumericType operator()(NumericType prev) const { return ++prev; }
};

template<typename NumericType>
struct NullStep
{
	NumericType operator()(NumericType prev) const { return prev; }
};


//constexpr const IncrementBy<int> IncrementByOne = IncrementBy<int>(1);
//
//template<typename NumericType>
//using IncrementByOne<NumericType> = Incr

template<typename NumericType, typename FunctionType>
class sequence_iterator
{
public:
	using value_type = NumericType;
	using pointer = value_type*;
	using const_pointer = value_type const*;
	using reference = value_type&;
	using const_reference = value_type const&;

private:
	value_type n {};
	value_type max {};
	FunctionType step;

//	bool inc_eq(sequence_iterator& iter) const { return n >= iter.n; }
//	bool dec_eq(sequence_iterator& iter) const { return n <= iter.n; }
//
//	bool increasing() const { return n < max; }
//	bool decreasing() const { return n > max; }
//	bool stationary() const { return !increasing() && !decreasing(); }

public:
	/**
	 * Generates a member of a number sequence on each iteration
	 * @param n beginning number of sequence
	 * @param max number of iterations
	 * @param step function to generate next number in sequence
	 */
	sequence_iterator(value_type n, value_type max
		, FunctionType step = IncrementBy<NumericType>(1))
	: n(n), max(max), step(step)
	{
	}

	sequence_iterator(sequence_iterator&& iter): n(iter.n), max(iter.max), step(iter.step)
	{
		iter.max = {}; // aka end()
	}

	sequence_iterator(const sequence_iterator& iter): n(iter.n), max(iter.max), step(iter.step) {}

	bool operator==(sequence_iterator& iter) const { return max == iter.max; }
	bool operator!=(sequence_iterator& iter) const { return !(*this == iter); }

	reference operator*() { return n; }
	const_reference operator*() const { return n; }

	sequence_iterator& operator++()
	{
		if(max)
		{
			n = step(n);
			--max;
		}
		return *this;
	}
	sequence_iterator operator++(int) { sequence_iterator iter(*this); ++(*this); return iter; }
};

template<typename NumericType, typename FunctionType>
sequence_iterator<NumericType, FunctionType> make_sequence_iterator(
	NumericType n, NumericType max, FunctionType step = IncrementBy<NumericType>(1))
{
	return sequence_iterator<NumericType, FunctionType>(n, max, step);
}

/**
 * A `sequence` is a virtual container that conceptually holds
 * a number sequence. It is accessed through iterators.
 */
template<typename NumericType, typename FunctionType>
struct sequence
{
public:
	using size_type = std::size_t;
	using value_type = NumericType;

protected:
	value_type b {};
	const value_type n {};
	FunctionType step;

public:
	/**
	 * Create a `sequence` object.
	 * @param b The beginning value of the sequence
	 * @param n The number of elements in the sequence
	 * @param step The function to generate a successive element
	 * from a previous element's value
	 */
	sequence(value_type b, value_type n, FunctionType step = IncrementBy<NumericType>(1))
	: b(b), n(n), step(step)
	{
	}

	size_type size() const { return n; }

	auto begin() const { return make_sequence_iterator(b, n, step); }
	auto end() const { return make_sequence_iterator(value_type{}, value_type{}, step); }
};

template<typename NumericType, typename FunctionType>
auto make_sequence(NumericType b
	, NumericType e
		, FunctionType step = IncrementBy<int>(1))
{
	return sequence<NumericType, FunctionType>(b, e, step);
}

template<typename NumericType>
auto make_sequence(NumericType b
	, NumericType e
		, int step = 1)
{
	return sequence<NumericType, IncrementBy<NumericType>>(b, e, IncrementBy<NumericType>(step));
}

/**
 * Convenience class for accessing containers by index.
 */
class indexes_of
: public sequence<long int, IncrementBy<long int>>
{
public:
	template<typename Container>
	indexes_of(Container const& c)
	: indexes_of(0, c.size(), 1) {bug_fun();}

	template<typename Container>
	indexes_of(Container const& c, value_type b)
	: indexes_of(b, c.size(), 1) {bug_fun();}

	template<typename Container>
	indexes_of(Container const&, value_type b, value_type e)
	: sequence(b, e - b,  IncrementBy<long int>(1)) {bug_fun();}

	template<typename Container>
	indexes_of(Container const&, value_type b, value_type e, long int step)
	: sequence(step<0?b-1:b, (step<0?e-1:e) - (step<0?b-1:b), IncrementBy<long int>(step))
	{
//		Ensures(step < 0 ? sequence::n <= sequence::b : sequence::b <= sequence::n);
//		Ensures(step < 0 ? (sequence::b >= -1 && sequence::n >= -1) : (sequence::b >= 0 && sequence::n >= 0));
	}
};

using indices_of = indexes_of;

// usage: int_seq(begin, end, step)
//
//	for(auto&& i: int_seq(-10, 20, 1))
//		bug_var(i);

} // hol

#endif // HOL_ITERATORS_H
