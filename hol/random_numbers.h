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

#include <random>
#include <cassert>
#include <limits>

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
 * 2) If you wich to use the 64bit version of the Mersenne Twister you might
 * do something like this:
 *
 * #define HOL_RANDOM_NUMBER_GENERATOR std::mt19937_64
 * #include <hol/random_numbers.h>
 *
 */

#ifndef HOL_RANDOM_DEVICE
#define HOL_RANDOM_DEVICE std::random_device
#endif
#ifndef HOL_RANDOM_DEVICE_SOURCE // (select implementation specific source)
#define HOL_RANDOM_DEVICE_SOURCE
#endif
#ifndef HOL_RANDOM_NUMBER_GENERATOR
#define HOL_RANDOM_NUMBER_GENERATOR std::mt19937
#endif

#ifndef HOL_CONCEPT
#ifdef __cpp_concepts
#define HOL_CONCEPT(c) c
#else
#define HOL_CONCEPT(c)
#endif
#endif // HOL_CONCEPT

namespace header_only_library {
namespace random_numbers {
namespace detail {

using Generator = HOL_RANDOM_NUMBER_GENERATOR;

#ifdef __cpp_concepts

template<typename Integer>
concept bool StdInteger
	 = std::is_same<Integer, short>::value
	|| std::is_same<Integer, int>::value
	|| std::is_same<Integer, long>::value
	|| std::is_same<Integer, long long>::value
	|| std::is_same<Integer, unsigned short>::value
	|| std::is_same<Integer, unsigned int>::value
	|| std::is_same<Integer, unsigned long>::value
	|| std::is_same<Integer, unsigned long long>::value;

template<typename Real>
concept bool StdReal
	 = std::is_same<Real, float>::value
	|| std::is_same<Real, double>::value
	|| std::is_same<Real, long double>::value;

template<typename InputIt>
concept bool ValuesConvertableToDouble
	 = std::is_convertible<typename std::iterator_traits<InputIt>::value_type,
		double>::value;

template <typename UnaryOperation>
concept bool RealCallableReal
	 = std::is_convertible<decltype(UnaryOperation(0.0)), double>::value;

template <typename UnaryOperation>
concept bool RetCallableType
	 = std::is_convertible<double, decltype(UnaryOperation(0.0))>::value;

template<typename Integer>
struct std_int_type_test {constexpr static bool value = true;};

template<typename Real>
struct std_real_type_test {constexpr static bool value = true;};

#else

template<typename Integer>
struct std_int_type_test
{
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

template<typename Real>
struct std_real_type_test
{
	constexpr static bool value =
	std::is_same<Real, float>::value
	|| std::is_same<Real, double>::value
	|| std::is_same<Real, long double>::value;
};

#endif // __cpp_concepts

//#if defined(__MINGW32__) || defined(__MINGW64__)
//#error Not compatable with MinGW due to a MinGW bug in std::random_device
//#else
//using random_device = std::random_device;
//#endif

inline
auto const& random_data()
{
	thread_local static std::array<std::random_device::result_type, Generator::state_size> data;
	thread_local static HOL_RANDOM_DEVICE rd{HOL_RANDOM_DEVICE_SOURCE};

	std::generate(std::begin(data), std::end(data), std::ref(rd));

	return data;
}

inline
Generator& random_generator()
{
	auto const& data = random_data();

	thread_local static std::seed_seq seeds(std::begin(data), std::end(data));
	thread_local static Generator gen{seeds};

	return gen;
}

template<typename Number>
Number random_number(Number from, Number to)
HOL_CONCEPT(requires detail::StdReal<Number>   )
HOL_CONCEPT(||       detail::StdInteger<Number>)
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

template<typename Distribution, typename... Args>
typename Distribution::result_type
randomly_distributed_number(Args&&... args)
{
	using param_type = typename Distribution::param_type;

	thread_local static Distribution dist;
	return dist(detail::random_generator(), param_type(std::forward<Args>(args)...));
}

} // namespace detail

/**
 * Get the underlying pseudo random number generator.
 * @return A random number generator.
 */
inline
detail::Generator& random_generator()
{
	return detail::random_generator();
}

/**
 * Re-seed the underlying pseudo random number generator
 * from a `std::seed_seq`.
 * Every unique list of seeds value will cause the same stream
 * of pseudo random numbers to be generated
 *
 * @param seeds The `std::seed_seq` to use.
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
	std::seed_seq seeds(std::begin(detail::random_data()), std::end(detail::random_data()));
	random_reseed(seeds);
}

/**
 * Re-seed the underlying pseudo random number generator.
 * Every unique list of seeds value will cause the same stream
 * of pseudo random numbers to be generated
 *
 * @param numbers The list of seed values to use.
 */
template<typename... Numbers>
void random_reseed(Numbers... numbers)
{
	std::seed_seq ss({numbers...});
	random_reseed(ss);
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
HOL_CONCEPT(requires detail::StdReal<Number>   )
HOL_CONCEPT(||       detail::StdInteger<Number>)
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
HOL_CONCEPT(requires detail::StdReal<Number>   )
HOL_CONCEPT(||       detail::StdInteger<Number>)
{
	Number from = {};
	auto mm = std::minmax(from, to);
	return detail::random_number(mm.first, mm.second);
}

/**
 * Generate a pseudo random number that is uniformly distributed
 * between the lowest and the highest possible value according to
 * the template parameter's numeric type.
 *
 * @param Number the type of number to generate.
 *
 * @return A random number between the lowest and the highest possible
 * value for the supplied numeric type (inclusive)
 */
template<typename Number>
Number random_number()
HOL_CONCEPT(requires detail::StdReal<Number>   )
HOL_CONCEPT(||       detail::StdInteger<Number>)
{
	auto from = std::numeric_limits<Number>::lowest();
	auto to = std::numeric_limits<Number>::max();
	return detail::random_number(from, to);
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
	{ return detail::randomly_distributed_number<std::bernoulli_distribution>(p); }

/**
 * Return an iterator to a randomly selected container element
 * from the `begin` iterator up to, but not including the `end` iterator.
 * If the distance between the iterators is zero, the behavior is undefined.
 *
 * @param begin The first iterator in the range to be considered.
 * @param end The last iterator in the range which will not be included.
 *
 * @return An iterator to a pseudo randomly selected element
 * from the supplied range.
 */
template<typename Iter>
Iter random_iterator(Iter begin, Iter end)
{
	assert(std::distance(begin, end) > 0);
	return std::next(begin,
		random_number(std::distance(begin, end) - 1));
}

/**
 * Return an iterator to a randomly selected container element. If the
 * container is empty the behavior is undefined.
 *
 * @param c The container to select an element from.
 *
 * @return An iterator to a pseudo randomly selected element from the supplied container.
 */
template<typename Container>
decltype(auto) random_iterator(Container&& c)
{
	assert(!c.empty());
	return random_iterator(std::begin(c), std::end(c));
}

/**
 * Return an iterator to a randomly selected array element. If the
 * array is empty the behavior is undefined.
 *
 * @param c The array to select an element from.
 *
 * @return An iterator to a pseudo randomly selected element from the supplied array.
 */
template<typename T, std::size_t N>
decltype(auto) random_iterator(T(&array)[N])
{
	assert(N > 0);
	return std::next(std::begin(array), random_number(N - 1));
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
decltype(auto) random_element(Container&& c)
{
	return *random_iterator(std::forward<Container>(c));
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
decltype(auto) random_element(Iter begin, Iter end)
{
	assert(std::distance(begin, end) > 0);
	return *random_iterator(begin, end);
}

} // namespace random_numbers
} // namespace header_only_library

#endif // HEADER_ONLY_LIBRARY_RANDOM_UTILS_H

