
inline
random_generator_type& random_generator()
{
	return detail::random_generator<random_generator_type>();
}

inline
void reseed(std::size_t n) { detail::reseed<random_generator_type>(n); }

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

	return detail::random_number<random_generator_type, Distribution, Number>(from, to);
}

template
<
	typename Distribution,
	typename Number = typename Distribution::result_type
>
Number random_number(Number from, Number to)
{
	return detail::random_number<random_generator_type, Distribution, Number>(from, to);
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

	return detail::random_number<random_generator_type, Distribution, Number>({}, to);
}

template
<
	typename Distribution,
	typename Number = typename Distribution::result_type
>
Number random_number(Number to)
{
	return detail::random_number<random_generator_type, Distribution, Number>({}, to);
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

	return detail::random_number<random_generator_type, Distribution, Number>({}, std::numeric_limits<Number>::max());
}

//template
//<
//	typename Distribution,
//	typename Number = typename Distribution::result_type
//>
//Number random_number()
//{
//	return detail::random_number<random_generator_type, Distribution, Number>({}, std::numeric_limits<Number>::max());
//}

/**
 * Randomly return true or false.
 * @return true or false
 */
inline
bool random_choice()
{
	thread_local static std::bernoulli_distribution dist;
	return dist(detail::random_generator<random_generator_type>());
}

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
		random_number<Distribution, std::size_t>(std::forward<Container>(c).size() - 1));
}

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

template<typename CharT>
std::basic_string<CharT> random_string(std::size_t n, CharT const* options)
{
	return random_string(n, std::basic_string<CharT>(options));
}

// require hol/unicode_utils.h
//std::string random_utf8_string(std::size_t n, std::string const& options)
//{
//	auto utf32_options = hol::utf8_to_utf32(options);
//	auto utf32 = hol::random_string(n, utf32_options);
//	return hol::utf32_to_utf8(utf32);
//}
//
//std::string shuffle_utf8_string(std::string const& s)
//{
//	auto utf32 = hol::utf8_to_utf32(s);
//	std::shuffle(std::begin(utf32), std::end(utf32), hol::random_generator());
//	return hol::utf32_to_utf8(utf32);
//}
