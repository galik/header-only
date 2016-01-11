#ifndef GALIK_U8STRING_H
#define GALIK_U8STRING_H
//	The MIT License (MIT)
//
//	Copyright (c) 2016 Galik <galik.bool@gmail.com>
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy of
//	this software and associated documentation files (the "Software"), to deal in
//	the Software without restriction, including without limitation the rights to
//	use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
//	of the Software, and to permit persons to whom the Software is furnished to do
//	so, subject to the following conditions:
//
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.

namespace galik {

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

	static unsigned size(const char* data)
	{
		return size(*data);
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

	void assign(const char* cp) noexcept { strncpy(this->cp, cp, size(cp)); }
	void assign(const u8char& u8c) noexcept { assign(u8c.data()); }

	unsigned size() const { return size(cp); }
	const char* data() const { return cp; }
	std::string string() const { return {cp, size()}; }

	bool operator==(const u8char& u8c) const { return u8c.size() == size() && !strncmp(u8c.cp, cp, size()); }
	bool operator!=(const u8char& u8c) const { return !(*this == u8c); }

	u8char& operator=(const char* cp) noexcept { assign(cp); return *this; }
	u8char& operator=(const u8char& u8c) noexcept { assign(u8c); return *this; }
	u8char& operator=(const std::string& s) noexcept { assign(s.data()); return *this; }
};

class u8string
{
	using iterator = std::vector<char>::iterator;
	using const_iterator = std::vector<char>::const_iterator;
	std::vector<char> utf8;

	template<typename InternalIterator>
	struct UTF8StringIterator
	{
		InternalIterator i;
		UTF8StringIterator(InternalIterator i): i(i) {}

		bool operator==(const UTF8StringIterator& i) const { return this->i == i.i; }
		bool operator!=(const UTF8StringIterator& i) const { return !(*this == i); }

		static u8char& get_u8c()
		{
			thread_local static u8char u8c;
			return u8c;
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

		u8char operator*() const { return u8char(&*i); }
		u8char* operator->() const { get_u8c() = &*i; return &get_u8c(); }
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

	u8char operator[](std::size_t n) const
	{
		auto i = begin();

		while(n--)
			++i;

		return *i;
	}

	std::string string() const { return {utf8.begin(), utf8.end()}; }

	// number of utf8 characters
	std::size_t size() const { return utf8.empty() ? 0 : size(utf8.data()); }

	UTF8StringIterator<iterator> begin() { return UTF8StringIterator<iterator>(utf8.begin()); }
	UTF8StringIterator<iterator> end() { return UTF8StringIterator<iterator>(utf8.end()); }

	UTF8StringIterator<const_iterator> begin() const { return UTF8StringIterator<const_iterator>(utf8.begin()); }
	UTF8StringIterator<const_iterator> end() const { return UTF8StringIterator<const_iterator>(utf8.end()); }
};

} // galik

#endif // GALIK_U8STRING_H
