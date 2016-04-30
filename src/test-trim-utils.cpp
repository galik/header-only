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

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "test.h"
#include "hol/string_utils.h"

using namespace hol;
using namespace std::literals::string_literals;

bool operator==(str_vec const& lhs, svw_vec const& rhs)
{
	if(lhs.size() != rhs.size())
		return false;
	for(auto i = 0U; i < lhs.size(); ++i)
		if(lhs[i] != rhs[i])
			return false;
	return true;
}

bool operator==(svw_vec const& lhs, str_vec const& rhs)
{
	return rhs == lhs;
}

std::istream& sgl(std::istream& is, std::string& line, char delim = '\n')
{
	return std::getline(is, line, delim);
}

std::istream& sgl(std::istream&& is, std::string& line, char delim = '\n')
{
	return sgl(is, line, delim);
}

template<typename Trimmer>
result_vec generic_trimmer_tests(Trimmer trim, str const& name, auto N = 1ULL)
{
	result_vec results;

	std::ifstream ifs;

	ifs.open("data/test-trim-utils-01.txt");

	str line;
	auto id = 0U;
	str::size_type pos = 0;
	auto line_number = 0U;

	while(std::getline(ifs, line))
	{
		++line_number;

		if((pos = line.find("//")) != str::npos)
			line.erase(pos);

		if(trim_mutator().round(line).empty())
			continue;

		//		config: "{" "}"
		//		L "{{{}}}" "}}}"
		//		L "{ {} }" " {} }"
		//		R "{{{}}}" "{{{"
		//		R "{ {} }" "{ {} "
		//		B "{{{}}}" ""
		//		B "{ {} }" " {} "

		if(!line.find("config:"))
		{
			str l, r;
			if(!sgl(sgl(sgl(sgl(std::istringstream(line) >> l, l, '"'), l, '"'), r, '"'), r, '"'))
				hol_throw_exception(std::runtime_error, "bad format line: " << line_number);

			trim.remove_from_left(l);
			trim.remove_from_right(r);
		}
		else if(!line.find_first_of("LRB"))
		{

			str from, to;
			if(!sgl(sgl(sgl(sgl(std::istringstream(line) >> from, from, '"'), from, '"'), to, '"'), to, '"'))
				hol_throw_exception(std::runtime_error, "bad format line: " << line_number);

			result r {name, id++};
			r.successful();
			for(decltype(N) i = 0; i < N; ++i)
			{
				auto fr = from;
				switch(line[0])
				{
					case 'L':
						if(trim.left(fr) != to)
							r.unsuccessful("Failed test line: "
								+ std::to_string(line_number) + " " + line
								+ " '" + std::string(trim.left(fr)) + "'");
					break;
					case 'R':
						if(trim.right(fr) != to)
							r.unsuccessful("Failed test line: "
								+ std::to_string(line_number) + " " + line
								+ " '" + std::string(trim.right(fr)) + "'");
					break;
					case 'B':
						if(trim.round(fr) != to)
							r.unsuccessful("Failed test line: "
								+ std::to_string(line_number) + " " + line
									+ " '" + std::string(trim.round(fr)) + "'");
					break;
				}
			}
			r.stop();

			results.push_back(r);
		}
	}

	return results;
}

int main()
{
	using namespace std::literals::string_literals;

	const auto N = 10000000ULL;

	try
	{
		std::vector<std::tuple<std::string, float>> all_times;
		result_vec all_results;

		result_vec results;
		results = generic_trimmer_tests(trim_mutator(), "mute", N);
		all_times.emplace_back("mute", std::accumulate(results.begin(), results.end(), 0.f, [](auto t, auto r){return r.time() + t;}));
		std::get<float>(all_times.back()) /= results.size();
		all_results.insert(all_results.end(), results.begin(), results.end());

		results = generic_trimmer_tests(trim_copier(), "copy", N);
		all_times.emplace_back("copy", std::accumulate(results.begin(), results.end(), 0.f, [](auto t, auto r){return r.time() + t;}));
		std::get<float>(all_times.back()) /= results.size();
		all_results.insert(all_results.end(), results.begin(), results.end());

		results = generic_trimmer_tests(trim_viewer(), "view", N);
		all_times.emplace_back("view", std::accumulate(results.begin(), results.end(), 0.f, [](auto t, auto r){return r.time() + t;}));
		std::get<float>(all_times.back()) /= results.size();
		all_results.insert(all_results.end(), results.begin(), results.end());

		std::stable_sort(all_results.begin(), all_results.end(), [](auto l, auto r){return l.id < r.id;});

		for(auto const& r: all_results)
			OUT(r.name << ": " << (r.id<10?"0":"") << r.id << ": " << (r?"P":"F") << " " << r.time() << (r?""s:" " + r.reason + " "));

		for(auto t: all_times)
			OUT(std::get<0>(t) << ": " << std::get<1>(t));
	}
	catch(const std::exception& e)
	{
		ERR(e.what());
	}
	catch(...)
	{
		ERR("Unknown exception thrown");
	}
}







