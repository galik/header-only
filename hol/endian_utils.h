#ifndef HEADER_ONLY_LIBRARY_ENDIAN_UTILS_H
#define HEADER_ONLY_LIBRARY_ENDIAN_UTILS_H

#include <cinttypes>
#include <algorithm>

namespace header_only_library {
namespace endian_utils {

constexpr bool is_littleendian()
{
	unsigned i{1};
	return ((char*)&i)[0];
}

constexpr bool is_bigendian()
{
	return !is_littleendian();
}

template<typename Numeric>
Numeric swap_endianness(Numeric n)
{
	std::reverse((char*)&n, ((char*)&n) + sizeof(n));
	return n;
}

template<typename Numeric>
Numeric from_littleendian(Numeric n)
{
	if(is_littleendian())
		return n;
	return swap_endianness(n);
}

template<typename Numeric>
Numeric from_bigendian(Numeric n)
{
	if(is_bigendian())
		return n;
	return swap_endianness(n);
}

} // namespace endian_utils
} // namespace header_only_library

#endif // HEADER_ONLY_LIBRARY_ENDIAN_UTILS_H
