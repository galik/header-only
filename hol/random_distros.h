#ifndef HEADER_ONLY_LIBRARY_RANDOM_DISTROS_H
#define HEADER_ONLY_LIBRARY_RANDOM_DISTROS_H
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

#include "random_numbers.h"

namespace header_only_library {
namespace random_numbers {

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

	assert(p > 0.0);
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

template<typename Integer = int>
Integer random_discrete(std::initializer_list<double> doubles)
{
	static_assert(detail::std_int_type_test<Integer>::value,
		"Template parameter must be short, int, long, long long, "
			"unsigned short, unsigned int, unsigned long, or unsigned long long");

	return detail::randomly_distributed_number<std::discrete_distribution<Integer>>(doubles);
}

template<typename Integer = int, typename InputIt>
Integer random_discrete(InputIt first, InputIt last)
{
	static_assert(detail::std_int_type_test<Integer>::value,
		"Template parameter must be short, int, long, long long, "
			"unsigned short, unsigned int, unsigned long, or unsigned long long");

	static_assert(std::is_convertible<
		typename std::iterator_traits<InputIt>::value_type, double>::value,
			"The iterator parameter's value types must be convertable to type double");

	return detail::randomly_distributed_number<std::discrete_distribution<Integer>>(first, last);
}

// http://en.cppreference.com/w/cpp/numeric/random/discrete_distribution
//
template<typename Integer = int, typename UnaryOperation>
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

template<typename Real = double, typename InputIteratorB, typename InputIteratorW>
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

template<typename Real = double, typename UnaryOperation>
Real random_piecewise_constant(std::initializer_list<Real> bl, UnaryOperation fw)
{
	static_assert(detail::std_real_type_test<Real>::value,
		"Parameters must be float, double or long double");

	return detail::randomly_distributed_number<std::piecewise_constant_distribution<Real>>(bl, fw);
}

template<typename Real = double, typename UnaryOperation>
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

template<typename Real = double, typename InputIteratorB, typename InputIteratorW>
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

template<typename Real = double, typename UnaryOperation>
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

template<typename Real = double, typename UnaryOperation>
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

} // namespace random_numbers
} // namespace header_only_library

#endif // HEADER_ONLY_LIBRARY_RANDOM_DISTROS_H

