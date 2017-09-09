#ifndef HEADER_ONLY_LIBRARY_ROPE_H
#define HEADER_ONLY_LIBRARY_ROPE_H

#include <deque>
#include <ostream>
#include <string>

#include "macro_utils.h"
#include "zstring_utils.h"

namespace header_only_library {
namespace rope_utils {

template<typename CharT>
class basic_rope
{
	using impl_type = std::deque<CharT>;

public:
	using size_type = typename impl_type::size_type;
	using value_type = CharT;
	using pointer = CharT*;
	using const_pointer = CharT const*;
	using reference = CharT&;
	using const_reference = CharT const&;

	using iterator = typename impl_type::iterator;
	using const_iterator = typename impl_type::const_iterator;

	static constexpr size_type npos = size_type(-1);

private:
	impl_type impl;

public:
	basic_rope() = default;
	basic_rope(basic_rope&&) = default;
	basic_rope(basic_rope const&) = default;
	basic_rope& operator=(basic_rope&&) = default;
	basic_rope& operator=(basic_rope const&) = default;

	// TODO: complete constructors
	basic_rope(std::basic_string<CharT> const& s) noexcept: impl(s.begin(), s.end()) {}
	basic_rope(size_type n, CharT c) noexcept: impl(n, c) {}

	basic_rope(basic_rope const& r, size_type pos): impl(r.begin() + pos, r.end()) {}
	basic_rope(basic_rope const& r, size_type pos, size_type n)
	{
		if(pos > r.size())
			throw std::out_of_range("bad index: " << to_string(pos) << " of " << to_string(impl.size()));
		impl.assign(r.begin() + pos, r.begin() + pos + n);
	}

	basic_rope(CharT const* s, size_type n): impl(s, s + n) {}
	basic_rope(CharT const* s) noexcept: basic_rope(s, std::strlen(s)) {}

	template< class InputIt >
	basic_rope(InputIt first, InputIt last): impl(first, last) {}
	basic_rope(std::initializer_list<CharT> il): basic_rope(il.begin(), il.end()) {}

	// end constructors

	template<typename Iter>
	basic_rope<CharT>& append(Iter first, Iter last)
	{
		impl.insert(impl.begin(), first, last);
		return *this;
	}

	basic_rope<CharT>& append(basic_rope<CharT> const& r)
	{
		impl.insert(impl.end(), r.begin(), r.end());
		return *this;
	}

	basic_rope<CharT>& append(std::basic_string<CharT> const& r)
	{
		impl.insert(impl.end(), r.begin(), r.end());
		return *this;
	}

	basic_rope<CharT>& append(CharT const* s)
	{
		while(*s)
			impl.push_back(*s++);
		return *this;
	}

	basic_rope<CharT>& append(CharT const* s, size_type n)
	{
		return append(s, s + n);
	}

	basic_rope<CharT>& append(std::initializer_list<CharT> il)
	{
		return append(il.begin(), il.end());
	}

	basic_rope<CharT>& append(size_type n, CharT c)
	{
		while(n--)
			append(c);
		return *this;
	}

	template<typename Iter>
	basic_rope<CharT>& assign(Iter first, Iter last)
	{
		impl.assign(first, last);
		return *this;
	}

	basic_rope<CharT>& assign(basic_rope<CharT>&& r)
	{
		impl.assign(std::move(r.impl));
		return *this;
	}

	basic_rope<CharT>& assign(basic_rope<CharT> const& r)
	{
		impl.assign(r.impl);
		return *this;
	}

	basic_rope<CharT>& assign(std::basic_string<CharT> const& r)
	{
		impl.assign(r.begin(), r.end());
		return *this;
	}

	basic_rope<CharT>& assign(std::basic_string<CharT> const& r, size_type pos, size_type n = npos)
	{
		using std::to_string;

		if(pos > impl.size())
			throw std::out_of_range("bad index: " << to_string(pos) << " of " << to_string(impl.size()));

		if(n == npos || !(pos + n < impl.size()))
			n = impl.size() - pos;

		impl.assign(r.begin() + pos, r.begin() + pos + n);
		return *this;
	}

	basic_rope<CharT>& assign(CharT const* s)
	{
		impl.assign(s, s + std::strlen(s));
		return *this;
	}

	basic_rope<CharT>& assign(CharT const* s, size_type n)
	{
		return assign(s, s + n);
	}

	basic_rope<CharT>& assign(std::initializer_list<CharT> il)
	{
		return assign(il.begin(), il.end());
	}

	basic_rope<CharT>& assign(size_type n, CharT c)
	{
		impl.clear();
		return append(n, c);
	}

	reference at(size_type pos) { return impl.at(pos); }
	const_reference at(size_type pos) const { return impl.at(pos); }

	reference back() { return impl.back(); }
	const_reference back(size_type pos) const { return impl.back(); }

	const_pointer c_str() const = delete; // non-contiguous
	pointer data() = delete; // non-contiguous
	const_pointer data() const = delete; // non-contiguous

	std::basic_string<CharT> string() const { return {impl.begin(), impl.end()}; }

	size_type capacity() const { return impl.capacity(); }

	void clear() { impl.clear(); }

	int compare(basic_rope<CharT> const& r) const
	{
		return std::lexicographical_compare(impl.begin(), impl.end(), r.impl.begin(), r.impl.end());
	}

	int compare(std::basic_string<CharT> const& s) const
	{
		return std::lexicographical_compare(impl.begin(), impl.end(), s.begin(), s.end());
	}

	int compare(CharT const* s) const
	{
		return std::lexicographical_compare(impl.begin(), impl.end(), s, s + std::strlen(s));
	}

	int compare(size_type pos, size_type n, const basic_rope& r) const
	{
		return basic_rope<CharT>(*this, pos, n).compare(r);
	}

	basic_rope<CharT> substr(size_type pos = 0, size_type count = npos) const
	{
		return {std::next(begin(), pos), std::next(begin(), pos + count)};
	}

	basic_rope<CharT>& insert(size_type pos, size_type n, CharT ch)
	{
		impl.insert(std::next(begin(), pos), n, ch);
		return *this;
	}

	basic_rope<CharT>& insert(size_type pos, CharT const* s)
	{
		using zstring_utils::generic::strlen;
		impl.insert(std::next(begin(), pos), s, s + strlen(s));
		return *this;
	}

	basic_rope<CharT>& insert(size_type pos, CharT const* s, size_type n)
	{
		using zstring_utils::generic::strnlen;
		impl.insert(std::next(begin(), pos), s, s + strnlen(s, n));
		return *this;
	}

	basic_rope<CharT>& insert(size_type pos, basic_rope<CharT> const& r)
	{
		impl.insert(std::next(begin(), pos), std::begin(r), std::end(r));
		return *this;
	}

	basic_rope<CharT>& insert(size_type pos, basic_rope<CharT> const& r,
		size_type r_pos, size_type n = npos)
	{
		HOL_ASSERT_MSG(r_pos < r.size(), "out of rope bounds: " << r_pos << " > " << r.size());

		auto r_size = r.size();

		if(n == npos)
			n = r_size - r_pos;

		if(r_pos + n > r.size() - 1)
			n = r_size - r_pos;

		auto r_beg = std::next(std::begin(r), r_pos);
		auto r_end = std::next(std::begin(r), r_pos + n);

		impl.insert(std::next(begin(), pos), r_beg, r_end);
		return *this;
	}

	iterator insert(iterator pos, CharT ch)
	{
		return *this;
	}

	iterator insert(const_iterator pos, CharT ch)
	{
		return *this;
	}

	void insert(iterator pos, size_type count, CharT ch)
	{
		return *this;
	}

	iterator insert(const_iterator pos, size_type count, CharT ch)
	{
		return *this;
	}

	template< class InputIt >
	void insert(iterator pos, InputIt first, InputIt last)
	{
		return *this;
	}

	template< class InputIt >
	iterator insert(const_iterator pos, InputIt first, InputIt last)
	{
		return *this;
	}

	iterator insert(const_iterator pos, std::initializer_list<CharT> ilist)
	{
		return *this;
	}

	template < class T >
	basic_rope<CharT>& insert(size_type pos, const T& t, size_type index_str, size_type count = npos)
	{
		return *this;
	}


	size_type size() const { return impl.size(); }

	auto begin() { return impl.begin(); }
	auto begin() const { return impl.begin(); }
	auto end() { return impl.end(); }
	auto end() const { return impl.end(); }

	auto cbegin() const { return impl.cbegin(); }
	auto cend() const { return impl.cend(); }

	//	template<typename CharT>
	//	basic_rope<CharT> subvec(basic_rope<CharT> const& v, size_type pos, size_type n = size_type(-1))
	//	{
	//		if(n == size_type(-1))
	//			n = v.size() - pos;
	//		return {v.begin() + pos, v.begin() + pos + n};
	//	}
	//

	friend
	std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT>& os, basic_rope<CharT> const& r)
	{
		for(auto c: r)
			os.put(c);
		return os;
	}

	bool operator==(basic_rope<CharT> const& other) const { return compare(other) == 0; }
	bool operator!=(basic_rope<CharT> const& other) const { return !(*this == other); }

//	template<typename CharU> friend
//	std::basic_ostream<CharU>& operator<<(std::basic_ostream<CharU>& os, basic_rope<CharU> const& r)
//	{
//		for(auto c: r)
//			os.put(c);
//		return os;
//	}
};

using rope = basic_rope<char>;
using wrope = basic_rope<wchar_t>;
using u16rope = basic_rope<char16_t>;
using u32rope = basic_rope<char32_t>;

namespace literals {

rope operator "" _r(char const* s, std::size_t n) { return rope{s, n}; }
wrope operator "" _wr(wchar_t const* s, std::size_t n) { return wrope{s, n}; }
u16rope operator "" _r16(char16_t const* s, std::size_t n) { return u16rope{s, n}; }
u32rope operator "" _r32(char32_t const* s, std::size_t n) { return u32rope{s, n}; }

} // namespace literals

} // namespace rope_utils
} // namespace header_only_library

//template<typename CharU>
//std::basic_ostream<CharU>& operator<<(std::basic_ostream<CharU>& os,
//	header_only_library::rope::basic_rope<CharU> const& r)
//{
//	for(auto c: r)
//		os.put(c);
//	return os;
//}


#endif // HEADER_ONLY_LIBRARY_ROPE_H
