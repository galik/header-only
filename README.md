# Header Only Library

Everything in this repository is released under the **MIT** license as follows:
```
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
```
# The Library

## This library is currently EXPERIMENTAL

* `string_utils`

This is basic string manipulation functions like `trim()`.

* `random_numbers`

This is basic random number functions making the post `C++11` random functions easier to use.

* `unicode_utils`

Basic Unicode conversion routines making the post `C++11` unicode conversion functions easier to use. 

## String Utils

Basic, efficiently implemented string handling utils for trimming, splitting, joining and replacing strings.

```cpp
#include <iostream>
#include <string>

#include <hol/string_utils.h>

namespace hol {using namespace header_only_library::string_utils;}

int main()
{
    std::string s = "   The quick brown fox    ";
    
    // The `mute` function change the passed in variable.
    hol::trim_mute(s); // remove surrounding whitespace (by default)
    
    char const* cs = "   Mary had a little lamb.   ";
    
    // The `copy` functions return a new copy of the trimmed string leaving
    // the original intact.
    s = hol::trim_copy(cs);
    
    auto v = hol::split_copy(s); // divide sentence into a std::vector of words
    
    for(auto const& s: v)
        std::cout << s << '\n';
}

```

## Random Numbers

The library is self-seeding, efficient and thread safe so it is east to use without complicated setup.

```cpp
#include <algorithm>
#include <iostream>
#include <vector>

#include <hol/random_numbers.h>

namespace hol {using namespace header_only_library::random_numbers;}

int main()
{
    // create between 1-20 (inclusive) elements
    std::vector<int> v(hol::random_number(1, 20));

    std::cout << "n: " << v.size() << '\n';

    // fill it with random numbers between 10 and 20
    std::generate(std::begin(v), std::end(v),
        []{ return hol::random_number(10, 20); });

    for_each(std::begin(v), std::end(v),
        [](auto i){ std::cout << i << ' '; });
    std::cout << '\n';

    // pick an element at random
    auto i = hol::random_element(v);

    std::cout << "i: " << i << '\n';

    // randomly copy about 75% of the values
    std::vector<int> w;

    std::copy_if(std::begin(v), std::end(v), std::back_inserter(w),
        [](auto){ return hol::random_choice(0.75); });

    std::cout << "n: " << w.size() << '\n';

    for_each(std::begin(w), std::end(w),
        [](auto i){ std::cout << i << ' '; });
    std::cout << '\n';
}
```

## Thread Utils <hol/thread_utils.h>

# locked_object<T>

The basic idea is very simple. There are just a few *wrapper* classes used to enforce read/write locking and, at the same time, presenting either a *const* (for read-only) or *non-const* (for read-write) *view* of the *wrapped* object.

The idea is to make it practically *compile-time impossible* to improperly access a resource shared between threads.

The *read-only* version (`reading_accessor`) contains an internal `shared_lock` and binds a *const reference* (`const&`) to the target object ensuring that **only** `const` functions can be invoked.

The *read_write* version (`writing_accessor`) contains an internal `unique_lock` and a *non-const reference* (`&`)  to the target allowing updates to its internal state.

**Here is some example usage:**

```cpp
locked_object<std::vector<int>> v;     // locked_object vector wrapper

v.push_back(3);                        // compile error

std::cout << v.size() << '\n';         // compile error

// locks are released at the end of their scope
{
	auto ro_v = v.open_for_reading();  // read-only locked wrapper

	ro_v->push_back(3);                // compile error

	std::cout << ro_v->size() << '\n'; // OK! (const functions accessible)
}

{
	auto rw_v = v.open_for_writing();  // read-or-write locked wrapper
	
	rw_v->push_back(3);                // OK! (non const functions accessible)
	
	std::cout << rw_v->size() << '\n'; // OK! (const functions accessible)
}

locked_object<std::size_t> i{0};       // locked_object integer wrapper

{
	// deadlock safe locking
	auto [ro_v, rw_i] = open_locked_objects(for_reading(v), for_writing(i));   
	
	ro_v->push_back(*rw_i);            // compile error (ro_v is read only)
	
	*rw_i = ro_v->size();              // OK!
}
```

## Z String Utils

This is an experimental library dealing with Zero-terminated Strings. It has two parts. First 
the generic versions of the classic `C` library string functions like `std::strlen`. 
Secondly functions with easier to understand names and better interfaces.

```cpp
```
