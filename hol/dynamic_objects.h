#ifndef HEADER_ONLY_LIBRARY_CONTAINERS_DYNAMIC_OBJECTS_H
#define HEADER_ONLY_LIBRARY_CONTAINERS_DYNAMIC_OBJECTS_H

#include <memory>
#include <type_traits>
#include <vector>

namespace header_only_library {
namespace dynamic_objects {

template<typename T>
class dynamic_object
{
public:
	template<typename... Args>
	dynamic_object(Args&&... args)
		: v(std::make_unique<T>(std::forward<Args>(args))...) {}

	dynamic_object()
		: v(std::make_unique<T>()) {}

	dynamic_object(T const& v)
		: v(std::make_unique<T>(v)) {}

	dynamic_object(T&& v)
		: v(std::make_unique<T>(std::move(v))) {}

	dynamic_object(dynamic_object const& other)
		: v(std::make_unique<T>(*other.v)) {}

	dynamic_object(dynamic_object&& other)
		: v(std::make_unique<T>(std::move(**other.v))) {}

	dynamic_object& operator=(T const& v)
		{ **this->v = v; return *this; }

	dynamic_object& operator=(T&& v)
		{ **this->v = std::move(v); return *this; }

	dynamic_object& operator=(dynamic_object const& other)
		{ **v = **other.v; return *this; }

	dynamic_object& operator=(dynamic_object&& other)
		{ **v = std::move(**other.v); return *this; }

	explicit operator T&() { return **v; }
	explicit operator T const&() const { return **v; }

	template<typename... Args>
	void emplace(Args&&... args) { v = std::make_unique<T>(std::forward<Args>(args)...); }

private:
	std::unique_ptr<T> v;
};

} // namespace dynamic_objects
} // namespace header_only_library

#endif // HEADER_ONLY_LIBRARY_CONTAINERS_DYNAMIC_OBJECTS_H
