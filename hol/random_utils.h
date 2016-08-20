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

namespace hol {

template<typename Integer = std::uint32_t>
class xorshift_engine
{
public:
	using result_type = Integer;

private:
	// 18446744073709551557
	static const result_type default_seed = 2147483647;

	result_type n[5];
	enum {x, y, z, w, t};

public:
	explicit xorshift_engine(result_type value = default_seed) noexcept
	{
		seed(value);
	}

	template<typename Sseq>
	explicit xorshift_engine(Sseq& seq)
	{
		seed(seq);
	}

	void seed(result_type value = default_seed) noexcept
	{
		n[x] = n[y] = n[z] = n[w] = value;
	}

	template<typename Sseq>
	void seed(Sseq& seq)
	{
		seq.generate(std::begin(n), std::end(n));
	}

	auto operator()()
	{
		n[t] = n[x];
		n[t] ^= n[t] << 11;
		n[t] ^= n[t] >> 8;
		n[x] = n[y];
		n[y] = n[z];
		n[z] = n[w];
		n[w] ^= n[w] >> 19;
		n[w] ^= n[t];
		return n[w];
	}

	void discard(unsigned long long z) { while(z--) (*this)(); }

	static constexpr result_type min() { return std::numeric_limits<result_type>::min(); }
	static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }

	template<typename UIntType>
	friend
	bool operator==(const xorshift_engine<UIntType>& lhs,
	                const xorshift_engine<UIntType>& rhs)
	{
		return lhs.n[x] == rhs.n[x]
			&& lhs.n[y] == rhs.n[y]
			&& lhs.n[z] == rhs.n[z]
			&& lhs.n[w] == rhs.n[w];
	}

	template<typename UIntType>
	friend
	bool operator!=(const xorshift_engine<UIntType>& lhs,
	               const xorshift_engine<UIntType>& rhs)
	{
		return !(lhs == rhs);
	}

	template<typename CharT, typename Traits, typename UIntType>
	friend
	std::basic_ostream<CharT, Traits>&
	operator<<(std::basic_ostream<CharT,Traits>& os,
		const xorshift_engine<UIntType>& e)
	{
		return os << e.n[x] << ' ' << e.n[y] << ' ' << e.n[z] << ' ' << e.n[w];
	}

	template<typename CharT, typename Traits, typename UIntType>
	friend
	std::basic_istream<CharT, Traits>&
	operator>>(std::basic_istream<CharT,Traits>& is,
		xorshift_engine<UIntType>& e)
	{
		return is >> e.n[x] >> e.n[y] >> e.n[z] >> e.n[w];
	}
};

using xorshift128 = xorshift_engine<std::uint32_t>;
using xorshift256 = xorshift_engine<std::uint64_t>;

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
	using rd = std::random_device;
	using param_type = typename Dist::param_type;
	std::seed_seq ss;
	Generator gen;
	Dist dist;

public:
	PRNG(): ss({rd{}(), rd{}(), rd{}(), rd{}()}), gen(ss) {}

	/**
	 * Get a random number between from and to, inclusively.
	 * @param from Lowest possible return value.
	 * @param to Highest possible return value.
	 * @return Random number in the range [from, to].
	 */
//	Type get(Type from = std::numeric_limits<Type>::lowest()
//		, Type to = std::numeric_limits<Type>::max())
//	{
//		return dist(gen, param_type{from, to});
//	}

	Type get()
	{
		return get(std::numeric_limits<Type>::lowest(), std::numeric_limits<Type>::max());
	}

	Type get(Type to)
	{
		return get(std::numeric_limits<Type>::min(), to);
	}

	Type get(Type from, Type to)
	{
		return dist(gen, param_type{from, to});
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

template<typename Type = std::int32_t>
using PRNG_32 = PRNG<Type>;

using PRNG_32S = PRNG_32<std::int32_t>;      // 32bit mt engine 32bit signed int
using PRNG_32U = PRNG_32<std::uint32_t>;     // 32bit mt engine 32bit unsigned int
using PRNG_32F = PRNG_32<float>;             // 32bit mt engine float
using PRNG_32D = PRNG_32<double>;            // 32bit mt engine double

template<typename Type = std::int64_t>
using PRNG_64 = PRNG<Type, std::mt19937_64>;

using PRNG_64S = PRNG_64<std::int64_t>;      // 64bit mt engine 64bit signed int
using PRNG_64U = PRNG_64<std::uint64_t>;     // 64bit mt engine 64bit unsigned int
using PRNG_64F = PRNG_64<float>;             // 64bit mt engine float
using PRNG_64D = PRNG_64<double>;            // 64bit mt engine double

//TODO: make PRNG use these tools?
template<typename Generator = std::mt19937>
struct random_tools
{
	using rd = std::random_device;
	std::seed_seq ss = {{rd{}(), rd{}(), rd{}(), rd{}()}};
	Generator gen;
	random_tools(): gen(ss) {}
};

template<typename Numeric>
Numeric random(Numeric from, Numeric to)
{
	thread_local random_tools<> tools;

	using dist_type = typename std::conditional
	<
		std::is_integral<Numeric>::value
		, std::uniform_int_distribution<Numeric>
		, std::uniform_real_distribution<Numeric>
	>::type;

	thread_local static dist_type dist;

	return dist(tools.gen, typename dist_type::param_type{from, to});
}

template<typename Numeric>
Numeric random(Numeric to = std::numeric_limits<Numeric>::max())
{
	return random({}, to);
}

} // hol

#endif // HOL_RANDOM_UTILS_H
