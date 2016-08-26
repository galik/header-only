# Header Only Library

Everything in this repository is released under the **MIT** license as follows:

    // The MIT License (MIT)
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

# The Library

* `basic_serialization`

This makes serializing simple objects easier by providing basic serialization functions.

* `string_stream_view`

This is a `std::istream` that wraps a `std::experimental::string_view` as its internal buffer.

* `u8string`

This is a `UTF-8` string class.

* `string_utils`

This is basic string manipulation functions like `trim()`.

* `random_utils`

This is basic random number functions

##Basic Serialization

Simple object serialization in both text and binary mode

Example usage:

```cpp
#include <hol/bug.h>
#include <hol/basic_serialization.h>

struct OtherType
{
	class SerializeMe* sm;
	int i1;
	int i2;

	OtherType(): sm(nullptr), i1(1), i2(2) {}

	friend std::ostream& operator<<(std::ostream& s, OtherType const& me)
	{
		using namespace hol::basic_serialization::txt;
		serialize_ptr(s, me.sm);
		serialize(s, me.i1);
		serialize(s, me.i2);

		return s;
	}

	friend std::istream& operator>>(std::istream& s, OtherType& me)
	{
		using namespace hol::basic_serialization::txt;
		deserialize_ptr(s, me.sm);
		deserialize(s, me.i1);
		deserialize(s, me.i2);

		return s;
	}
};

struct SerializeMe
{
	int i;
	OtherType* otp1;
	std::string s;
	OtherType* otp2;

	friend std::ostream& operator<<(std::ostream& s, SerializeMe const& me)
	{
		using namespace hol::basic_serialization::txt;
		serialize(s, me.i);
		serialize_ptr(s, me.otp1);
		serialize(s, me.s);
		serialize_ptr(s, me.otp2);

		return s;
	}

	friend std::istream& operator>>(std::istream& s, SerializeMe& me)
	{
		using namespace hol::basic_serialization::txt;
		deserialize(s, me.i);
		deserialize_ptr(s, me.otp1);
		deserialize(s, me.s);
		deserialize_ptr(s, me.otp2);

		return s;
	}
};

int main()
{
	SerializeMe me;
	me.i = 4;
	me.s = "hello";
	me.otp1 = new OtherType;
	me.otp2 = new OtherType;
	me.otp1->sm = &me;
	if(me.otp1)
	{
		me.otp1->i1 = 3;
		me.otp1->i2 = 5;
	}
	if(me.otp2)
	{
		me.otp2->i1 = 6;
		me.otp2->i2 = 12;
	}
	
	std::stringstream ss;

	using namespace hol::basic_serialization::txt;
	serialization_init();
	ss << me;

	bug_var(ss.str());

	SerializeMe you;

	using namespace hol::basic_serialization::txt;
	serialization_init();
	ss >> you;

	bug_var(you.i);
	bug_var(you.s);
	if(me.otp1)
	{
		bug_var(you.otp1->i1);
		bug_var(you.otp1->i2);
	}
	if(me.otp2)
	{
		bug_var(you.otp2->i1);
		bug_var(you.otp2->i2);
	}

	bug_var(you.otp1->sm->i);
	bug_var(you.otp1->sm->s);

	bug_var(you.otp1->sm->otp1->i1);
	bug_var(you.otp1->sm->otp2);
}
```