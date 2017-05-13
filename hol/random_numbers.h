#ifndef HEADER_ONLY_LIBRARY_RANDOM_UTILS_H
#define HEADER_ONLY_LIBRARY_RANDOM_UTILS_H
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

#ifndef HOL_RANDOM_UTILS_GENERATOR
#define HOL_RANDOM_UTILS_GENERATOR std::mt19937
#endif

namespace header_only_library {
namespace random_numbers {
namespace detail {

using Generator = HOL_RANDOM_UTILS_GENERATOR;

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

inline
auto const& random_data()
{
	thread_local static std::array<typename Generator::result_type, Generator::state_size> data;
	thread_local static std::random_device rd;
	std::generate(std::begin(data), std::end(data), std::ref(rd));
	return data;
}

inline
auto& random_generator()
{
	thread_local static std::seed_seq seeds(std::begin(random_data()), std::end(random_data()));
	thread_local static Generator gen{seeds};

	return gen;
}

inline
void random_reseed()
{
	std::seed_seq seeds(std::begin(random_data()), std::end(random_data()));
	random_generator().seed(seeds);
}

inline
void random_reseed(typename Generator::result_type n)
{
	std::seed_seq seeds(&n, &n + 1);
	random_generator().seed(seeds);
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
 * Properly re-seed the underlying pseudo random number
 * generator from the std::random_device.
 */
inline
void random_reseed() { detail::random_reseed(); }

/**
 * Re-seed the underlying pseudo random number generator.
 * Every unique seed value will cause the same stream of pseudo random numbers
 * to be generated
 *
 * @param n The seed value to use.
 */
inline
void random_reseed(std::size_t n) { detail::random_reseed(n); }

/**
 * Generate a pseudo random number that is uniformly distributed
 * between two values.
 *
 * @param from Lowest possible value.
 * @param to Highest possible value.
 *
 * @return A random number between `from` and `to` (inclusive)
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
{
	auto from = std::numeric_limits<Number>::lowest();
	auto to = std::numeric_limits<Number>::max();
	return detail::random_number(from, to);
}

// bernoulli_distribution

inline
bool random_bernoulli(double p = 0.5)
{
	return detail::randomly_distributed_number<std::bernoulli_distribution>(p);
}

template<typename Integer = int>
Integer random_binomial(Integer t = 1, double p = 0.5)
{
	static_assert(detail::std_int_type_test<Integer>::value,
		"Template parameter must be short, int, long, long long, "
			"unsigned short, unsigned int, unsigned long, or unsigned long long");

	assert(0 <= t);
	assert(0.0 < p);
	assert(p <= 1.0);
	// Requires that 0 ≤ p ≤ 1 and 0 ≤ t.
	return detail::randomly_distributed_number<std::binomial_distribution<Integer>>(t, p);
}

template<typename Integer = int>
Integer random_negative_binomial(Integer k = 1, double p = 0.5)
{
	static_assert(detail::std_int_type_test<Integer>::value,
		"Template parameter must be short, int, long, long long, "
			"unsigned short, unsigned int, unsigned long, or unsigned long long");

	assert(0 < k);
	assert(0.0 < p);
	assert(p <= 1.0);

	return detail::randomly_distributed_number<std::negative_binomial_distribution<Integer>>(k, p);
}

template<typename Integer = int>
Integer random_geometric(double p = 0.5)
{
	static_assert(detail::std_int_type_test<Integer>::value,
		"Template parameter must be short, int, long, long long, "
			"unsigned short, unsigned int, unsigned long, or unsigned long long");

	assert(0.0 < p);
	assert(p < 1.0);

	return detail::randomly_distributed_number<std::geometric_distribution<Integer>>(p);
}

template<typename Integer = int>
Integer random_poisson(double mean = 1.0)
{
	static_assert(detail::std_int_type_test<Integer>::value,
		"Template parameter must be short, int, long, long long, "
			"unsigned short, unsigned int, unsigned long, or unsigned long long");

	assert(0.0 < mean);

	return detail::randomly_distributed_number<std::poisson_distribution<Integer>>(mean);
}

template<typename Real = double>
Real random_exponential(Real lambda = 1.0)
{
	static_assert(detail::std_real_type_test<Real>::value,
		"Parameter must be float, double or long double");

	assert(0.0 < lambda);

	return detail::randomly_distributed_number<std::exponential_distribution<Real>>(lambda);
}

template<typename Real = double>
Real random_gamma(Real alpha = 1.0, Real beta = 1.0)
{
	static_assert(detail::std_real_type_test<Real>::value,
		"Parameters must be float, double or long double");

	return detail::randomly_distributed_number<std::gamma_distribution<Real>>(alpha, beta);
}

template<typename Real = double>
Real random_weibull(Real a = 1.0, Real b = 1.0)
{
	static_assert(detail::std_real_type_test<Real>::value,
		"Parameters must be float, double or long double");

	return detail::randomly_distributed_number<std::weibull_distribution<Real>>(a, b);
}

template<typename Real = double>
Real random_extreme_value(Real a = 0.0, Real b = 1.0)
{
	static_assert(detail::std_real_type_test<Real>::value,
		"Parameters must be float, double or long double");

	return detail::randomly_distributed_number<std::extreme_value_distribution<Real>>(a, b);
}

template<typename Real = double>
Real random_normal(Real mean = 0.0, Real standard_deviation = 1.0)
{
	static_assert(detail::std_real_type_test<Real>::value,
		"Parameters must be float, double or long double");

	return detail::randomly_distributed_number<std::normal_distribution<Real>>(mean, standard_deviation);
}

template<typename Real = double>
Real random_lognormal(Real m = 0.0, Real s = 1.0)
{
	static_assert(detail::std_real_type_test<Real>::value,
		"Parameters must be float, double or long double");

	return detail::randomly_distributed_number<std::lognormal_distribution<Real>>(m, s);
}

template<typename Real = double>
Real random_chi_squared(Real n = 1.0)
{
	static_assert(detail::std_real_type_test<Real>::value,
		"Parameters must be float, double or long double");

	return detail::randomly_distributed_number<std::chi_squared_distribution<Real>>(n);
}

template<typename Real = double>
Real random_cauchy(Real a = 0.0, Real b = 1.0)
{
	static_assert(detail::std_real_type_test<Real>::value,
		"Parameters must be float, double or long double");

	return detail::randomly_distributed_number<std::cauchy_distribution<Real>>(a, b);
}

template<typename Real = double>
Real random_fisher_f(Real m = 1.0, Real n = 1.0)
{
	static_assert(detail::std_real_type_test<Real>::value,
		"Parameters must be float, double or long double");

	return detail::randomly_distributed_number<std::fisher_f_distribution<Real>>(m, n);
}

template<typename Real = double>
Real random_student_t(Real n = 1.0)
{
	static_assert(detail::std_real_type_test<Real>::value,
		"Parameters must be float, double or long double");

	return detail::randomly_distributed_number<std::student_t_distribution<Real>>(n);
}

// TODO: Param pack not reflected in the underlying interface... remove this opton?
//template<typename Integer, typename... Doubles>
//Integer random_discrete(Doubles&&... doubles)
//{
//	static_assert(detail::std_int_type_test<Integer>::value,
//		"Template parameter must be short, int, long, long long, "
//			"unsigned short, unsigned int, unsigned long, or unsigned long long");
//
//	// TODO: static text to make sure Doubles are correctly typed
//
//	return detail::randomly_distributed_number<std::discrete_distribution<Integer>>(
//		std::initializer_list<double>{std::forward<Doubles>(doubles)...});
//}

template<typename Integer = int>
Integer random_discrete(std::initializer_list<double> doubles)
{
	static_assert(detail::std_int_type_test<Integer>::value,
		"Template parameter must be short, int, long, long long, "
			"unsigned short, unsigned int, unsigned long, or unsigned long long");

	// TODO: static text to make sure Doubles are correctly typed

	return detail::randomly_distributed_number<std::discrete_distribution<Integer>>(doubles);
}

template<typename InputIt, typename Integer = int>
Integer random_discrete(InputIt first, InputIt last)
{
	static_assert(detail::std_int_type_test<Integer>::value,
		"Template parameter must be short, int, long, long long, "
			"unsigned short, unsigned int, unsigned long, or unsigned long long");

	// TODO: static text to make sure Doubles are correctly typed

	return detail::randomly_distributed_number<std::discrete_distribution<Integer>>(first, last);
}

// http://en.cppreference.com/w/cpp/numeric/random/discrete_distribution
//
template<typename UnaryOperation, typename Integer = int>
Integer random_discrete(std::size_t count, double xmin, double xmax, UnaryOperation unary_op)
{
	static_assert(detail::std_int_type_test<Integer>::value,
		"Template parameter must be short, int, long, long long, "
			"unsigned short, unsigned int, unsigned long, or unsigned long long");

	if(count == 0)
		return 0;

	double delta = (xmax - xmin) / double(count);

	assert(delta > 0.0);

	return detail::randomly_distributed_number<std::discrete_distribution<Integer>>(
		count, xmin, xmax, unary_op);
}

// piecewise_constant_distribution
// http://en.cppreference.com/w/cpp/numeric/random/piecewise_constant_distribution

template<typename InputIteratorB, typename InputIteratorW, typename Real = double>
Real random_piecewise_constant(InputIteratorB first_b, InputIteratorB last_b, InputIteratorW first_w)
{
	static_assert(detail::std_real_type_test<Real>::value,
		"Parameters must be float, double or long double");

	static_assert(
		std::is_convertible<typename std::iterator_traits<InputIteratorB>::value_type, double>::value
		||std::is_convertible<typename std::iterator_traits<InputIteratorW>::value_type, double>::value,
		"Iterator value_type must be convertible to type double");

	using dist_type = std::piecewise_constant_distribution<Real>;

	return detail::randomly_distributed_number<dist_type>(first_b, last_b, first_w);
}

template<typename UnaryOperation, typename Real = double>
Real random_piecewise_constant(std::initializer_list<Real> bl, UnaryOperation fw)
{
	static_assert(detail::std_real_type_test<Real>::value,
		"Parameters must be float, double or long double");

	return detail::randomly_distributed_number<std::piecewise_constant_distribution<Real>>(bl, fw);
}

template<typename UnaryOperation, typename Real = double>
Real random_piecewise_constant(std::size_t nw, Real xmin, Real xmax, UnaryOperation fw)
{
	static_assert(detail::std_real_type_test<Real>::value,
		"Parameters must be float, double or long double");

	static_assert(
		std::is_convertible<decltype(fw(0.0)), double>::value,
		"Functor return type must be convertible to type double");

	using dist_type = std::piecewise_constant_distribution<Real>;

	assert(xmin < xmax);
	assert(0.0 < (xmax - xmin) / Real(nw ? nw : 1));

	return detail::randomly_distributed_number<dist_type>(nw, xmin, xmax, fw);
}

// piecewise_linear_distribution
// C++11 26.5.8.6.3
//

template<typename InputIteratorB, typename InputIteratorW, typename Real = double>
Real random_piecewise_linear(InputIteratorB first_b, InputIteratorB last_b, InputIteratorW first_w)
{
	static_assert(detail::std_real_type_test<Real>::value,
		"Parameters must be float, double or long double");

	static_assert(
		std::is_convertible<typename std::iterator_traits<InputIteratorB>::value_type, double>::value
		||std::is_convertible<typename std::iterator_traits<InputIteratorW>::value_type, double>::value,
		"Iterator value_type must be convertible to type double");

	using dist_type = std::piecewise_linear_distribution<Real>;

	return detail::randomly_distributed_number<dist_type>(first_b, last_b, first_w);
}

template<typename UnaryOperation, typename Real = double>
Real random_piecewise_linear(std::initializer_list<Real> bl, UnaryOperation fw)
{
	static_assert(detail::std_real_type_test<Real>::value,
		"Parameters must be float, double or long double");

	static_assert(
		std::is_convertible<decltype(fw(0.0)), double>::value,
		"Functor return type must be convertible to type double");

	using dist_type = std::piecewise_linear_distribution<Real>;

	return detail::randomly_distributed_number<dist_type>(bl, fw);
}

template<typename UnaryOperation, typename Real = double>
Real random_piecewise_linear(std::size_t nw, Real xmin, Real xmax, UnaryOperation fw)
{
	static_assert(detail::std_real_type_test<Real>::value,
		"Parameters must be float, double or long double");

	static_assert(
		std::is_convertible<decltype(fw(0.0)), double>::value,
		"Functor return type must be convertible to type double");

	using dist_type = std::piecewise_linear_distribution<Real>;

	return detail::randomly_distributed_number<dist_type>(nw, xmin, xmax, fw);
}

// -------------------------

/**
 * Randomly return true or false.
 *
 * @return true or false
 */
template<typename Real = double>
bool random_choice(Real p = 0.5) { return random_bernoulli(p); }

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
	return std::next(std::begin(std::forward<Container>(c)),
		random_number(std::forward<Container>(c).size() - 1));
}

/**
 * Return a randomly selected container element. If the
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
 * Create a pseudo randomly generates String from a provided String of
 * characters.
 *
 * @param n The number of characters to include.
 * @param options The String of characters to choose from while building
 * the returned String.
 *
 * @return A pseudo randomly generated string of n characters from the characters
 * supplied in the options parameter.
 */
template<typename String>
String random_string(std::size_t n, String const& options)
{
	static_assert(
		std::is_same<String, std::string>::value
		|| std::is_same<String, std::wstring>::value
		|| std::is_same<String, std::u16string>::value
		|| std::is_same<String, std::u32string>::value,
		"Unsupported string type");

	String s;
	s.reserve(n);

	while(n--)
		s.push_back(random_element(options));

	return s;
}

/**
 * Create a pseudo randomly generates String from a provided array of
 * characters.
 *
 * @param n The number of characters to include.
 * @param options The array of characters to choose from while building the String.
 *
 * @return A pseudo randomly generated string of n characters from the characters
 * supplied in the options parameter.
 */
template<typename CharT>
std::basic_string<CharT> random_string(std::size_t n, CharT const* options)
{
	return random_string(n, std::basic_string<CharT>(options));
}

} // namespace random_utils
} // namespace header_only_library

#endif // HEADER_ONLY_LIBRARY_RANDOM_UTILS_H

