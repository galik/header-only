#ifndef HEADER_ONLY_LIBRARY_RANDOM_UTILS_H
#define HEADER_ONLY_LIBRARY_RANDOM_UTILS_H

#include <random>
#include <cassert>
#include <limits>

namespace header_only_library {
namespace random_utils {
namespace detail {

template<typename Engine>
constexpr std::size_t get_state_size(decltype(Engine::state_size)) { return Engine::state_size; }

template<typename Engine>
constexpr std::size_t get_state_size() { return 0; }

template<typename Engine>
auto const& random_data()
{
	constexpr std::size_t state_size = get_state_size<Engine>();
	thread_local static std::array<typename Engine::result_type, state_size ? state_size:1> data;
	std::random_device rd;
	std::generate(std::begin(data), std::end(data), std::ref(rd));
	return data;
}

template<typename Engine>
auto& random_engine()
{
	thread_local static std::seed_seq seeds(std::begin(random_data<Engine>()), std::end(random_data<Engine>()));
	thread_local static Engine eng{seeds};

	return eng;
}

template<typename Engine>
void reseed(std::size_t n)
{
	std::seed_seq ss(&n, &n + 1);
	random_engine<Engine>().seed(ss);
}

template<typename Engine, typename Distribution, typename Number>
Number random_number(Number from, Number to)
{
	thread_local static Distribution dist;
	return dist(random_engine<Engine>(), typename Distribution::param_type{from, to});
}

} // namespace detail

namespace mersenne_twister_32 {
using random_engine_type = std::mt19937;
#include "random_utils_bits.h"
} // namespace mersenne_twister_32

namespace mersenne_twister_64 {
using random_engine_type = std::mt19937_64;
#include "random_utils_bits.h"
} // namespace mersenne_twister_64

namespace linear_congruential {
using random_engine_type = std::minstd_rand;
#include "random_utils_bits.h"
} // linear_congruential

namespace linear_congruential_0 {
using random_engine_type = std::minstd_rand0;
#include "random_utils_bits.h"
} // linear_congruential_0

namespace subtract_with_carry_24 {
using random_engine_type = std::ranlux24_base;
#include "random_utils_bits.h"
} // namespace subtract_with_carry_24

namespace subtract_with_carry_48 {
using random_engine_type = std::ranlux48_base;
#include "random_utils_bits.h"
} // namespace subtract_with_carry_48

#endif // HEADER_ONLY_LIBRARY_RANDOM_UTILS_H

} // namespace random_utils
} // namespace header_only_library
