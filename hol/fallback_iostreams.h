#ifndef HEADER_ONLY_LIBRARY_FALLBACK_IOSTREAMS_H
#define HEADER_ONLY_LIBRARY_FALLBACK_IOSTREAMS_H
//
// Copyright (c) 2018 Galik <galik.bool@gmail.com>
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

#include <filesystem>
#include <iostream>
#include <memory>
#include <stdexcept>

namespace header_only_library {
namespace iostream_utils {

namespace fs = std::filesystem;

template<typename CharT>
class basic_cin_fallback_input_stream
: public std::basic_istream<CharT>
{
public:
	basic_cin_fallback_input_stream(): std::basic_istream<CharT>(std::cin.rdbuf()) {}

	basic_cin_fallback_input_stream(fs::path const& pathname): basic_cin_fallback_input_stream()
		{ open(pathname); }

	~basic_cin_fallback_input_stream()
		{ close(); }

	/**
	 * Close any files then open input stream. Throws on failure.
	 * @param pathname Path name of file to open.
	 */
	void open(fs::path const& pathname)
	{
		close();
		auto fb = std::make_unique<std::filebuf>();
		fb->open(pathname, std::ios::in);
		if(!fb->is_open())
			throw std::runtime_error(pathname.string() + ": " + std::strerror(errno));
		std::basic_istream<CharT>::rdbuf(fb.release());
	}

	/**
	 * Close any files then open input stream. Falls back to std::cin on failure.
	 * @param pathname Path name of file to open.
	 */
	void try_to_open(fs::path const& pathname)
	{
		close();
		auto fb = std::make_unique<std::filebuf>();
		fb->open(pathname, std::ios::in);
		if(fb->is_open())
			std::basic_istream<CharT>::rdbuf(fb.release());
	}

	void close()
	{
		if(std::basic_istream<CharT>::rdbuf() != std::cin.rdbuf())
			delete std::basic_istream<CharT>::rdbuf();
		std::basic_istream<CharT>::rdbuf(std::cin.rdbuf());
	}
};

template<typename CharT>
class basic_cout_fallback_output_stream
: public std::basic_ostream<CharT>
{
public:
	basic_cout_fallback_output_stream(): std::basic_ostream<CharT>(std::cout.rdbuf()) {}
	basic_cout_fallback_output_stream(fs::path const& pathname): basic_cout_fallback_output_stream()
		{ open(pathname); }

	~basic_cout_fallback_output_stream()
		{ close(); }

	/**
	 * Close any files then open output stream. Throws on failure.
	 * @param pathname Path name of file to open.
	 */
	void open(fs::path const& pathname)
	{
		close();
		auto fb = std::make_unique<std::filebuf>();
		fb->open(pathname, std::ios::in);
		if(!fb->is_open())
			throw std::runtime_error(pathname.string() + ": " + std::strerror(errno));
		std::basic_ostream<CharT>::rdbuf(fb.release());
	}

	/**
	 * Close any files then open output stream. Falls back to std::cout on failure.
	 * @param pathname Path name of file to open.
	 */
	void try_to_open(fs::path const& pathname)
	{
		close();
		auto fb = std::make_unique<std::filebuf>();
		fb->open(pathname, std::ios::in);
		if(fb->is_open())
			std::basic_ostream<CharT>::rdbuf(fb.release());
	}

	void close()
	{
		if(std::basic_ostream<CharT>::rdbuf() != std::cout.rdbuf())
			delete std::basic_ostream<CharT>::rdbuf();
		std::basic_ostream<CharT>::rdbuf(std::cout.rdbuf());
	}
};

using cin_fallback_input_stream = basic_cin_fallback_input_stream<char>;
using wcin_fallback_input_stream = basic_cin_fallback_input_stream<wchar_t>;

using cout_fallback_output_stream = basic_cout_fallback_output_stream<char>;
using wcout_fallback_output_stream = basic_cout_fallback_output_stream<wchar_t>;

} // namespace iostream_utils
} // namespace header_only_library

#endif // HEADER_ONLY_LIBRARY_FALLBACK_IOSTREAMS_H
