#ifndef HOL_SMART_OBJECTS_H
#define HOL_SMART_OBJECTS_H
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

#include <new>
#include <cstddef>

namespace header_only_library {
namespace smart_objects {

template<typename T>
class shared_ptr
{
	struct ref_type
	{
		T* o;
		int c;
		ref_type(T* o): o(o), c(1) {}
		~ref_type() { delete o; }
	};

public:
	shared_ptr(shared_ptr const& xsp) noexcept: ref(xsp.ref) { increment_ref(); }
	~shared_ptr() { decrement_ref(); }

	shared_ptr& operator=(const shared_ptr& xsp) noexcept
	{
		decrement_ref();
		ref = xsp.ref;
		increment_ref();
		return *this;
	}

	std::size_t count() const { return ref->c; }

	template<typename... Args>
	static shared_ptr<T> make(Args&&... args)
	{
		auto type = new T(std::forward<Args>(args)...);

		if(auto ref = new (std::nothrow) shared_ptr<T>::ref_type(type))
			return shared_ptr<T>(ref);

		delete type;
		throw std::bad_alloc();
	}

private:
	shared_ptr(ref_type* ref) noexcept: ref(ref) {}

	void increment_ref() { ++ref->c; }
	void decrement_ref() { if(!--ref->c) delete ref; }

	ref_type* ref;
};

} // namespace smart_objects
} // namespace header_only_library

#endif // HOL_SMART_OBJECTS_H
