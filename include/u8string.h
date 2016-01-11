#ifndef GALIK_U8STRING_H
#define GALIK_U8STRING_H

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

#include <array>
#include <cstring>

namespace galik {

using std::strncpy;
using std::strcmp;
using std::strlen;

using char16_pair = std::array<char16_t, 2>;

class u8char
{
	friend class u8string;

	enum u8mask
	{
		  na1 = 0b10000000 // !(c & na1)
		, na2 = 0b00100000
		, na3 = 0b00010000
		, na4 = 0b00001000
		, na5 = 0b00000100
		, na6 = 0b00000010
		, no2 = 0b11000000 // (c & no2) == no2
		, no3 = 0b11100000
		, no4 = 0b11110000
		, no5 = 0b11111000
		, no6 = 0b11111100
	};

	char cp[6] = {}; // code point

	static unsigned is_continuation(char c)
	{
		return (c & 0b10000000) && !(c & 0b01000000);
	}

	static unsigned size(const unsigned char c)
	{
		if(!(c & na1))
			return 1;
		if(!(c & na2) && ((c & no2) == no2))
			return 2;
		if(!(c & na3) && ((c & no3) == no3))
			return 3;
		if(!(c & na4) && ((c & no4) == no4))
			return 4;
		if(!(c & na5) && ((c & no5) == no5))
			return 5;
		if(!(c & na6) && ((c & no6) == no6))
			return 5;
		return 0;
	}

public:
	u8char() {}
	explicit u8char(const char* cp) noexcept { assign(cp); }
	u8char(const u8char& u8c) noexcept { assign(u8c); }

	// static utils

	static unsigned size(const char* data)
	{
		return size(*data);
	}

	static char32_t char32(const char* cp)
	{
		auto sz = u8char::size(cp);

		if(sz == 1)
			return *cp;

		char32_t c32 = (0b01111111 >> sz) & (*cp);

		for(unsigned i = 1; i < sz; ++i)
			c32 = (c32 << 6) | (cp[i] & 0b00111111);

		return c32;
	}

	static unsigned char16(const char* cp, char16_pair& cp16)
	{
		char32_t c32 = u8char::char32(cp);

		if(c32 < 0xD800 || (c32 > 0xDFFF && c32 < 0x10000))
		{
			cp16[0] = char16_t(c32);
			cp16[1] = 0;
			return 1;
		}

		c32 -= 0x010000;

		cp16[0] = ((0b11111111110000000000 & c32) >> 10) + 0xD800;
		cp16[1] = ((0b00000000001111111111 & c32) >> 00) + 0xDC00;

		return 2;
	}

	void assign(const char* cp) noexcept { strncpy(this->cp, cp, size(cp)); }
	void assign(const u8char& u8c) noexcept { assign(u8c.data()); }

	unsigned size() const { return size(cp); }

	const char* data() const { return cp; }
	std::string string() const { return {cp, size()}; }
	char32_t char32() const { return char32(cp); }
	unsigned char16(char16_pair& cp16) const { return char16(cp, cp16); }

	bool operator==(const u8char& u8c) const { return u8c.size() == size() && !strncmp(u8c.cp, cp, size()); }
	bool operator!=(const u8char& u8c) const { return !(*this == u8c); }

	u8char& operator=(const char* cp) noexcept { assign(cp); return *this; }
	u8char& operator=(const u8char& u8c) noexcept { assign(u8c); return *this; }
	u8char& operator=(const std::string& s) noexcept { assign(s.data()); return *this; }

	char* begin() { return cp; }
	char* end() { return cp + size(cp); }

	const char* begin() const { return cp; }
	const char* end() const { return cp + size(cp); }


};

class u8string
{
	using container = std::string;
	using container_ref = container&;
	using container_const_ref = container const&;
	using iterator = container::iterator;
	using const_iterator = container::const_iterator;

	container utf8;

	template<typename ContainerRef, typename Iterator>
	class u8char_proxy
	{
		ContainerRef c;
		Iterator i;

	public:
		u8char_proxy(ContainerRef c, Iterator i): c(c), i(i) {}
		u8char_proxy(const u8char_proxy& u8p): c(u8p.c), i(u8p.i) {}

		u8char_proxy& operator=(u8char u8c)
		{
			iterator e = i + u8char::size(*i);
			c.replace(i, e, u8c.begin(), u8c.end());
			return *this;
		}

		operator u8char() const { return u8char(&*i); }

		std::size_t size() const { return u8char::size(*i); }
		std::string string() const { return {&*i, &*(i + size())}; }
		char32_t char32() const { return u8char::char32(&*i); }
	};

	template<typename ContainerRef, typename Iterator>
	class UTF8StringIterator
	{
		friend class u8string;
		ContainerRef c;
		Iterator i;

		static u8char& get_u8c(const u8char& u8c)
		{
			thread_local static u8char local_u8c;
			local_u8c = u8c;
			return local_u8c;
		}

		static auto& get_u8c_proxy(ContainerRef c, Iterator i)
		{
			thread_local static u8char_proxy<ContainerRef, Iterator> local_u8c_proxy;
			local_u8c_proxy = u8char_proxy<ContainerRef, Iterator>(c, i);
			return local_u8c_proxy;
		}

		void increment()
		{
			i += u8char::size(*i);
		}

		void decrement()
		{
			--i;
			while(u8char::is_continuation(*i))
				--i;
		}

	public:
		UTF8StringIterator(ContainerRef c, Iterator i): c(c), i(i) {}

		bool operator==(const UTF8StringIterator& i) const { return this->i == i.i; }
		bool operator!=(const UTF8StringIterator& i) const { return !(*this == i); }

		auto operator*() { return u8char_proxy<ContainerRef, Iterator>(c, i); }
		auto* operator->() { return &get_u8c_proxy(c, i); }

		u8char operator*() const { return u8char(&*i); }
		u8char* operator->() const { return &get_u8c(&*i); }

		UTF8StringIterator& operator++() { increment(); return *this; }
		UTF8StringIterator operator++(int) { UTF8StringIterator n = i; increment(); return n; }

		UTF8StringIterator& operator--() { decrement(); return *this; }
		UTF8StringIterator operator--(int) { UTF8StringIterator n = i; decrement(); return n; }
	};

	// number of utf8 characters
	static std::size_t size(const char* s)
	{
		std::size_t n = 0;
		for(; *s; s += u8char::size(s))
			++n;
		return n;
	}

public:
	u8string() {}
	u8string(const char* s): utf8(s, s + strlen(s)) {}
	u8string(const std::string& s): utf8(s.begin(), s.end()) {}

	auto begin() { return UTF8StringIterator<container_ref, iterator>(utf8, utf8.begin()); }
	auto end() { return UTF8StringIterator<container_ref, iterator>(utf8, utf8.end()); }

	auto begin() const { return UTF8StringIterator<container_const_ref, const_iterator>(utf8, utf8.begin()); }
	auto end() const { return UTF8StringIterator<container_const_ref, const_iterator>(utf8, utf8.end()); }

	auto operator[](std::size_t n)
	{
		auto i = begin();

		while(n--)
			++i;

		return u8char_proxy<container_ref, iterator>(utf8, i.i);
	}

	u8char operator[](std::size_t n) const
	{
		auto i = begin();

		while(n--)
			++i;

		return *i;
	}

	std::string string() const { return {utf8.begin(), utf8.end()}; }
	std::u16string u16string() const { throw std::runtime_error("Not implemented: u8string::u16string()"); return {}; } // TODO:
	std::u32string u32string() const
	{
		std::u32string utf32;

		for(auto&& u8c: *this)
			utf32 += u8c.char32();

		return utf32;
	}

	// number of utf8 characters
	std::size_t size() const { return utf8.empty() ? 0 : size(utf8.data()); }
};

} // galik

#endif // GALIK_U8STRING_H
