
inline
random_engine_type& random_engine()
{
	return detail::random_engine<random_engine_type>();
}

inline
void reseed(std::size_t n) { detail::reseed<random_engine_type>(n); }

/**
 * Generate a random number between two values.
 * @param from Lowest possible value.
 * @param to Highest possible value.
 * @return A random number between `from` and `to` (inclusive)
 */
template<typename Number>
Number random_number(Number from, Number to)
{
	using Distribution = typename std::conditional
	<
		std::is_integral<Number>::value,
		std::uniform_int_distribution<Number>,
		std::uniform_real_distribution<Number>
	>::type;

	return detail::random_number<random_engine_type, Distribution, Number>(from, to);
}

template
<
	typename Distribution,
	typename Number = typename Distribution::result_type
>
Number random_number(Number from, Number to)
{
	return detail::random_number<random_engine_type, Distribution, Number>(from, to);
}

/**
 * Generate a random number from zero to the specified value.
 * @param to Highest possible value.
 * @return A random number between `0` and `to` (inclusive)
 */
template<typename Number>
Number random_number(Number to)
{
	using Distribution = typename std::conditional
	<
		std::is_integral<Number>::value,
		std::uniform_int_distribution<Number>,
		std::uniform_real_distribution<Number>
	>::type;

	return detail::random_number<random_engine_type, Distribution, Number>({}, to);
}

template
<
	typename Distribution,
	typename Number = typename Distribution::result_type
>
Number random_number(Number to)
{
	return detail::random_number<random_engine_type, Distribution, Number>({}, to);
}

template<typename Number>
Number random_number()
{
	using Distribution = typename std::conditional
	<
		std::is_integral<Number>::value,
		std::uniform_int_distribution<Number>,
		std::uniform_real_distribution<Number>
	>::type;

	return detail::random_number<random_engine_type, Distribution, Number>({}, std::numeric_limits<Number>::max());
}

//template
//<
//	typename Distribution,
//	typename Number = typename Distribution::result_type
//>
//Number random_number()
//{
//	return detail::random_number<random_engine_type, Distribution, Number>({}, std::numeric_limits<Number>::max());
//}

/**
 * Randomly return true or false.
 * @return true or false
 */
inline
bool random_choice()
{
	thread_local static std::bernoulli_distribution dist;
	return dist(detail::random_engine<random_engine_type>());
}

// TODO: random_element is broken

/**
 * Return a randomly selected container element.
 * @param c The container to select an element from.
 * @return A random element from the supplied container.
 */
template
<
	typename Distribution = std::uniform_int_distribution<std::size_t>,
	typename Container = void
>
auto& random_element(Container&& c)
{
	assert(!c.empty());
	return *std::next(std::begin(std::forward<Container>(c)),
		random_number<std::size_t, Distribution>(std::forward<Container>(c).size() - 1));
}

