#ifndef HOL_RANDOM_UTILS_H
#define HOL_RANDOM_UTILS_H
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
#include <limits>
#include <cassert>
#include <algorithm>

namespace hol {
namespace random_utils {
namespace rnd {
namespace detail {

//template<class T = std::mt19937, std::size_t N = T::state_size>
//auto ProperlySeededRandomEngine () -> typename std::enable_if<!!N, T>::type {
//    typename T::result_type random_data[N];
//    std::random_device source;
//    std::generate(std::begin(random_data), std::end(random_data), std::ref(source));
//    std::seed_seq seeds(std::begin(random_data), std::end(random_data));
//    T seededEngine (seeds);
//    return seededEngine;
//}

template<typename Generator = std::mt19937>
class random_tools
{
public:
	Generator gen = seeded_rng();

private:
	// fully seed the random number generator
	std::mt19937 seeded_rng()
	{
		typename Generator::result_type data[Generator::state_size];
		std::random_device rd;
		std::generate(std::begin(data), std::end(data), std::ref(rd));
		std::seed_seq seeds(std::begin(data), std::end(data));
		return Generator(seeds);
	}
};

} // detail

/// return a thread-local mersenne twister prng (32-bit)
inline std::mt19937& mt32()
{
	thread_local static detail::random_tools<std::mt19937> tools;
	return tools.gen;
}

inline bool random_choice()
{
	thread_local static std::bernoulli_distribution dist;
	return dist(mt32());
}

template<typename Numeric>
Numeric random_number(Numeric from, Numeric to)
{
	static_assert(std::is_integral<Numeric>::value || std::is_floating_point<Numeric>::value,
		"random_number<Numeric> must be called with an integer or a floating point number");

	using dist_type = typename std::conditional
	<
		std::is_integral<Numeric>::value
		, std::uniform_int_distribution<Numeric>
		, std::uniform_real_distribution<Numeric>
	>::type;

	thread_local static dist_type dist;

	return dist(mt32(), typename dist_type::param_type{from, to});
}

template<typename Numeric>
Numeric random_number(Numeric to = std::numeric_limits<Numeric>::max())
{
	return random_number({}, to);
}

template<typename Container>
decltype(auto) random_element(Container&& c)
{
	assert(!c.empty());
	return std::forward<Container>(c)[rnd::random_number(std::forward<Container>(c).size() - 1)];
}

/**
 * Returns a random unsigned integer of type `std::size_t` with a value
 * between `0` and the one fewer than the number of supplied parameters.
 * @param weights Probability weights of the parameter's position being
 * returned as a `std::size_t` value.
 * @return The *zero based* position of one of the supplied parameters.
 */
template<typename... Weights>
inline std::size_t random_weighted_position(Weights... weights)
{
	thread_local static std::discrete_distribution<std::size_t> dist{double(weights)...};
	return dist(rnd::mt32());
}

/**
 * Usage:
 *
 * PRNG_32S prng;
 *
 * if(prng.percent(20))
 * {
 *     // Do this 20% of the time
 * }
 *
 * if(prng.chances(2, 5))
 * {
 *     // Do this 2 out of 5 times
 * }
 *
 * int i = prng.get(2, 8); // number between 2 and 8 inclusive
 *
 */
template
	<
	  typename Type = int
	, typename Generator = std::mt19937
	, typename Dist = typename std::conditional
		<
			std::is_integral<Type>::value
			, std::uniform_int_distribution<Type>
			, std::uniform_real_distribution<Type>
		>::type
	>
class PRNG
{
	using param_type = typename Dist::param_type;
	detail::random_tools<Generator> tools;
	Dist dist;

public:

	/**
	 * Get a random number of any value according to
	 * the limits of the PRNG's constructed numerical type.
	 * @return Full numerical range random number.
	 */
	Type get()
	{
		return get(std::numeric_limits<Type>::lowest(), std::numeric_limits<Type>::max());
	}

	/**
	 * Get a random number between 0 and `to`, inclusively.
	 * @param to Highest possible return value.
	 * @return Random number in the range [0, to].
	 */
	Type get(Type to)
	{
		return get(std::numeric_limits<Type>::min(), to);
	}

	/**
	 * Get a random number between from and to, inclusively.
	 * @param from Lowest possible return value.
	 * @param to Highest possible return value.
	 * @return Random number in the range [from, to].
	 */
	Type get(Type from, Type to)
	{
		return dist(tools.gen, param_type{from, to});
	}

	/**
	 * Chances of success. This function will return true
	 * num times out of denom.
	 * @param num
	 * @param denom
	 * @return true if num is greater than a random number between 1 and denom
	 */
	bool chances(Type num, Type denom) { return num > get(1, denom); }

	/**
	 * Percentage chance of success. This function returns true
	 * num percent of the time.
	 * @param num
	 * @return true num% of the time.
	 */
	bool percent(Type num) { return chances(num, 100); }
};

/// 32bit mt engine
template<typename Type = std::int32_t>
using PRNG_32 = PRNG<Type>;

/// 32bit mt engine 32bit signed int
using PRNG_32S = PRNG_32<std::int32_t>;

/// 32bit mt engine 32bit unsigned int
using PRNG_32U = PRNG_32<std::uint32_t>;

/// 32bit mt engine float
using PRNG_32F = PRNG_32<float>;

/// 32bit mt engine double
using PRNG_32D = PRNG_32<double>;

/// 64bit mt engine
template<typename Type = std::int64_t>
using PRNG_64 = PRNG<Type, std::mt19937_64>;

/// 64bit mt engine 64bit signed int
using PRNG_64S = PRNG_64<std::int64_t>;

/// 64bit mt engine 64bit unsigned int
using PRNG_64U = PRNG_64<std::uint64_t>;

/// 64bit mt engine float
using PRNG_64F = PRNG_64<float>;

/// 64bit mt engine double
using PRNG_64D = PRNG_64<double>;

} // rnd
} // random_utils
} // hol

#endif // HOL_RANDOM_UTILS_H
