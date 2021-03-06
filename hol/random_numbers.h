#ifndef HEADER_ONLY_LIBRARY_RANDOM_NUMBERS_H
#define HEADER_ONLY_LIBRARY_RANDOM_NUMBERS_H
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
#include <array>
#include <cassert>
#include <chrono>
#include <limits>
#include <random>
#include <thread>

//#include "assertions.h"
//#include "misc_utils.h"

/**
 * Configuration:
 *
 * std::random_device Does not work on MinGW so another one may be selected
 * by setting the macro `HOL_RANDOM_DEVICE` to another qualified class.
 *
 * std::random_device allows different non-deterministic sources to be selected
 * through a parameter to its constructor. This parameter can be set using the
 * macro `HOL_RANDOM_DEVICE_SOURCE`.
 *
 * The Pseudo Random Number Generator may be selected by setting the macro
 * `HOL_RANDOM_NUMBER_GENERATOR`. Otherwise it defaults to std::mt19937.
 *
 * Examples:
 *
 * 1) Typically the default values do not need changing. On MinGW, because
 * `std::random_device` is broken you might do something like this:
 *
 * #include <boost/random/random_device.hpp>
 * #define HOL_RANDOM_DEVICE boost::random_device
 * #include <hol/random_numbers.h>
 *
 * 2) If you wish to use the 64bit version of the Mersenne Twister you might
 * do something like this:
 *
 * #define HOL_RANDOM_NUMBER_GENERATOR std::mt19937_64
 * #include <hol/random_numbers.h>
 *
 */

#ifndef HOL_RANDOM_DEVICE
#  ifdef __MINGW32__
#    include <chrono>
     namespace header_only_library {
     namespace random_numbers {
     namespace detail {
     struct random_device
     {
	     using result_type = std::size_t;
	     std::size_t operator()() const
	     {
		     std::chrono::system_clock::time_point then{};
		     return std::size_t((std::chrono::system_clock::now() - then).count());
	     }
     };
     } // namespace detail
     } // namespace random_numbers
     } // namespace header_only_library
#    define HOL_RANDOM_DEVICE ::header_only_library::random_numbers::detail::random_device
#  else
#    define HOL_RANDOM_DEVICE std::random_device
#  endif
#endif
#ifndef HOL_RANDOM_DEVICE_SOURCE // (select implementation specific source)
#  define HOL_RANDOM_DEVICE_SOURCE
#endif
#ifndef HOL_RANDOM_NUMBER_GENERATOR
#  define HOL_RANDOM_NUMBER_GENERATOR std::mt19937
#endif

#ifndef HOL_AT_LEAST_CPP17
#  if __cplusplus >= 201703L
#    define HOL_AT_LEAST_CPP17
#  endif
#endif // HOL_AT_LEAST_CPP17

namespace header_only_library {

/*! \namespace random_numbers
    \brief Random number functions, making modern C++ random number generation simple.
*/
namespace random_numbers {
namespace detail {

using Generator = HOL_RANDOM_NUMBER_GENERATOR;

/*! \struct std_int_type_test
	\brief SFNAE test for integer types
 */
template<typename Integer>
struct std_int_type_test
{
	/*! true or false */
	constexpr static bool value =
	std::is_same<Integer, short>::value
	|| std::is_same<Integer, int>::value
	|| std::is_same<Integer, long>::value
	|| std::is_same<Integer, long long>::value
	|| std::is_same<Integer, unsigned short>::value
	|| std::is_same<Integer, unsigned int>::value
	|| std::is_same<Integer, unsigned long>::value
	|| std::is_same<Integer, unsigned long long>::value;
};

/*! \struct std_real_type_test
	\brief SFNAE test for float types
 */
template<typename Real>
struct std_real_type_test
{
	/*! true or false */
	constexpr static bool value =
	std::is_same<Real, float>::value
	|| std::is_same<Real, double>::value
	|| std::is_same<Real, long double>::value;
};

inline
void random_reseed(Generator& gen)
{
	auto constexpr gen_size = sizeof(Generator)/sizeof(HOL_RANDOM_DEVICE::result_type);
	std::array<HOL_RANDOM_DEVICE::result_type, gen_size> data;
	HOL_RANDOM_DEVICE rd{};
	std::generate(std::begin(data), std::end(data), std::ref(rd));
	std::seed_seq seeds(std::begin(data), std::end(data));
	gen.seed(seeds);
}

inline
Generator& random_generator()
{
	thread_local static Generator gen = []{
		Generator gen;
		random_reseed(gen);
		return gen;
	}();

	return gen;
}

template<typename Number>
Number random_number(Number from, Number to)
{
	static_assert(std_int_type_test<Number>::value||std_real_type_test<Number>::value,
		"Parameters must be integer or floating point numbers");

	using Distribution = typename std::conditional
	<
		std::is_integral<Number>::value,
		std::uniform_int_distribution<Number>,
		std::uniform_real_distribution<Number>
	>::type;

	thread_local static Distribution dist;

	return dist(random_generator(), typename Distribution::param_type{from, to});
}

template<typename Number>
Number random_number()
{
	static_assert(std_int_type_test<Number>::value||std_real_type_test<Number>::value,
		"Parameters must be integer or floating point numbers");

	using Distribution = typename std::conditional
	<
		std::is_integral<Number>::value,
		std::uniform_int_distribution<Number>,
		std::uniform_real_distribution<Number>
	>::type;

	thread_local static Distribution dist;

	return dist(random_generator());
}

template<typename Distribution, typename... Args>
typename Distribution::result_type
randomly_distributed_number(Args&&... args)
{
	using param_type = typename Distribution::param_type;

	thread_local static Distribution dist;
	return dist(detail::random_generator(), param_type(std::forward<Args>(args)...));
}

} // namespace detail

/*! \fn random_generator
    \brief Get the underlying pseudo random number generator.

    \return The random number generator used by this library.
*/
inline
detail::Generator& random_generator()
{
	return detail::random_generator();
}

/*!
 * \brief Re-seed the underlying pseudo random number generator
 * from a `std::seed_seq`.
 * Every unique list of seeds value will cause the same stream
 * of pseudo random numbers to be generated
 *
 * \param seeds The `std::seed_seq` to use.
 */
inline
void random_reseed(std::seed_seq& seeds)
{
	random_generator().seed(seeds);
}

/**
 * Properly re-seed the underlying pseudo random number
 * generator from the std::random_device.
 */
inline
void random_reseed()
{
	detail::random_reseed(random_generator());
}

/**
 * Re-seed the underlying pseudo random number generator.
 * Every unique list of seeds value will cause the same stream
 * of pseudo random numbers to be generated
 *
 * @param numbers The list of seed values to use.
 */
template<typename Number, typename... Numbers>
void random_reseed(Number number, Numbers... numbers)
{
	std::seed_seq ss({number, numbers...});
	random_reseed(ss);
}

template<typename Number>
void random_reseed(Number number)
{
	random_generator().seed(number);
}

/**
 * Generate a pseudo random number that is uniformly distributed
 * between two values.
 *
 * @param from Lowest possible value.
 * @param to Highest possible value.
 *
 * @return A random number between `from` and `to`
 * (inclusive for integers, exclusive for floats)
 */
template<typename Number>
Number random_number(Number from, Number to)
{
	auto mm = std::minmax(from, to);
	return detail::random_number(mm.first, mm.second);
}

/**
 * Generate a pseudo random number that is uniformly distributed
 * between zero and the specified (positive or negative) value.
 *
 * @param to Highest possible value.
 *
 * @return A random number between `0` and `to` (inclusive)
 */
template<typename Number>
Number random_number(Number to)
{
	Number from = {};
	auto mm = std::minmax(from, to);
	return detail::random_number(mm.first, mm.second);
}

/**
 * Generate a pseudo random number that is uniformly distributed
 * between 0.0 and 1.0 (exclusive) for real values and 0 and the maximum
 * possible value (inclusive) for integer numbers.
 *
 * @param Number the type of number to generate.
 *
 * @return A random number between 0.0 and 1.0
 */
template<typename Number>
Number random_number()
{
	return detail::random_number<Number>();
}

/**
 * Randomly return true or false.
 *
 * @param p The probability that the function will return `true`.
 *
 * @return true or false
 */
template<typename Real = double>
bool random_choice(Real p = 0.5)
{
	assert(p >= 0.0);
	assert(p <= 1.0);
	return detail::randomly_distributed_number<std::bernoulli_distribution>(p);
}

/**
 * Return an iterator to a randomly selected container element
 * from the `begin` iterator up to, and including the `end` iterator.
 *
 * @param begin The first iterator in the range to be considered.
 * @param end The last iterator in the range.
 *
 * @return An iterator to a pseudo randomly selected element
 * from the supplied range (or the `end` iterator).
 */
template<typename Iter>
Iter random_iterator(Iter begin, Iter end)
{
	return std::next(begin,
		random_number(std::distance(begin, end)));
}

/**
 * Return a randomly selected iterator to container.
 *
 * @param c The container to select an iterator from.
 *
 * @return A randomly selected iterator, including std::end(c).
 */
template<typename Container>
auto random_iterator(Container& c) -> decltype(std::begin(c))
{
	assert(!c.empty());
	return random_iterator(std::begin(c), std::end(c));
}

/**
 * Return a randomly selected iterator to an array.
 *
 * @param array The array to select an element from.
 *
 * @return A randomly selected iterator, including std::end(array).
 */
template<typename T, std::size_t N>
auto random_iterator(T(&array)[N]) -> decltype(std::begin(array))
{
	assert(N > 0);
	return std::next(std::begin(array), random_number(N));
}

/**
 * Return a randomly selected element from a container. If the
 * container is empty the behavior is undefined.
 *
 * @param c The container to select an element from.
 *
 * @return A reference to a pseudo randomly selected element from the supplied container.
 */
template<typename Container>
auto random_element(Container&& c) -> decltype(*std::begin(c))
{
	return *random_iterator(std::begin(std::forward<Container>(c)),
		std::prev(std::end(std::forward<Container>(c))));
}

/**
 * Return a randomly selected element from a range of iterators. If the
 * distance between the iterators is zero,  the behavior is undefined.
 *
 * @param begin The first iterator in the range to be considered.
 * @param end The last iterator in the range which will not be included.
 *
 * @return A reference to a pseudo randomly selected element from the supplied container.
 */
template<typename Iter>
auto random_element(Iter begin, Iter end) -> decltype(*begin)
{
	assert(std::distance(begin, end) > 0);
	return *random_iterator(begin, std::prev(end));
}

template<typename Iter>
void random_shuffle(Iter begin, Iter end)
{
	std::shuffle(begin, end, random_generator());
}

template<typename Container>
void random_shuffle(Container& c)
{
	std::shuffle(std::begin(c), std::end(c), random_generator());
}

template<typename T, std::size_t N>
void random_shuffle(T(&array)[N])
{
	std::shuffle(std::begin(array), std::end(array), random_generator());
}

template<typename Duration1, typename Duration2>
void random_sleep_for(Duration1 min, Duration2 max)
{
	using common_duration_type = typename std::common_type<Duration1, Duration2>::type;
	auto duration = common_duration_type(random_number(min.count(), max.count()));
	std::this_thread::sleep_for(duration);
}

#ifdef HOL_AT_LEAST_CPP17

template<typename PIter, typename SIter>
auto random_sample(PIter begin, PIter end, SIter out, std::size_t n)
	{ std::sample(begin, end, out, n, random_generator()); }

template<typename Container>
auto random_sample(Container&& c, std::size_t n)
{
	if(n >= c.size())
		return c;

	typename std::remove_reference<Container>::type s;
	s.reserve(n);
	random_sample(std::begin(std::forward<Container>(c)), std::end(std::forward<Container>(c)),
		std::inserter(s, std::end(s)), n);
	return s;
}

#endif // C++17

// Alternative experimentation
namespace alt {

template<typename Generator = std::mt19937, typename RandomDevice = std::random_device>
class random_tools
{
public:
	random_tools() = delete;

	static Generator& generator()
	{
		thread_local static Generator generator{RandomDevice{}()};
		return generator;
	}

	template<typename Unsigned = std::size_t>
	static void seed(Unsigned n)
	{
		generator().seed(n);
	}

	template<typename Int = int>
	static Int uniform_int(Int lo, Int hi)
	{
		thread_local static std::uniform_int_distribution<Int> d;
		return d(generator(), std::uniform_int_distribution<Int>::param_type(lo, hi));
	}

	template<typename Int = int>
	static Int uniform_int(Int hi)
	{
		return uniform_int(Int(), hi);
	}

	template<typename Int = int>
	static Int uniform_int()
	{
		return uniform_int(std::numeric_limits<Int>::lowest(), std::numeric_limits<Int>::max());
	}

	template<typename Real = double>
	static Real uniform_real(Real lo, Real hi)
	{
		thread_local static std::uniform_real_distribution<Real> d;
		return d(generator(), std::uniform_real_distribution<Real>::param_type(lo, hi));
	}

	template<typename Real = double>
	static Real uniform_real(Real hi)
	{
		return uniform_real(Real(), hi);
	}

	template<typename Real = double>
	static Real uniform_real()
	{
		return uniform_real(std::numeric_limits<Real>::lowest(), std::numeric_limits<Real>::max());
	}

	static bool bernoulli(double p = 0.5)
	{
		thread_local static std::bernoulli_distribution d;
		return d(generator(), std::bernoulli_distribution::param_type{p});
	}
};

using mt = std::conditional<sizeof(void*)==8,random_tools<std::mt19937_64>,random_tools<std::mt19937>>::type;
using mt32 = random_tools<std::mt19937>;
using mt64 = random_tools<std::mt19937_64>;

template<typename Unsigned = unsigned>
class fifty_fifty_distribution
{
public:
	using result_type = bool;
	struct param_type { using distribution_type = fifty_fifty_distribution<Unsigned>; };

	fifty_fifty_distribution() = default;
	fifty_fifty_distribution(param_type p): fifty_fifty_distribution() {}

	template<typename RNG>
	result_type operator()(RNG& rng, param_type p = {})
	{
		if(idx == (bytes.size() * 8))
		{
			idx = 0;
			auto r = rng();
			bytes.resize(sizeof(r));
			std::copy((char const*) &r, (char const*) (&r + 1), bytes.data());
		}

		bool b = bytes[idx / 8] & (1 << (idx % 8));
		++idx;
		return b;
	}

	constexpr bool min() const { return false; }
	constexpr bool max() const { return true; }

	void reset() { bytes.clear(); idx = 0; }

	constexpr param_type param() const { return {}; }

	template<typename U = unsigned>
	friend std::ostream& operator<<(std::ostream& os, fifty_fifty_distribution<U> const& d)
	{
		os << d.bytes.size() << '\n';
		for(auto const c: d.bytes)
			os.put(c);
		os << d.idx << '\n';
		return os;
	}

	template<typename U = unsigned>
	friend std::istream& operator>>(std::istream& is, fifty_fifty_distribution<U>& d)
	{
		d.bytes.clear();
		std::size_t n;
		is >> n;
		is.ignore();
		for(char c; n--;)
			d.bytes.push_back(c);
		is >> d.idx;
		is.ignore();
		return is;
	}

private:
	std::vector<char> bytes;
	unsigned idx = 0;
};

} // namespace alt
} // namespace random_numbers
} // namespace header_only_library

#endif // HEADER_ONLY_LIBRARY_RANDOM_UTILS_H

