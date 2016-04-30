#ifndef HOL_CLI_ARGS_H
#define HOL_CLI_ARGS_H
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

namespace hol { namespace arg {

template<typename Iter>
class found_type
{
	Iter found;
	Iter end;

public:
	found_type() {}
	found_type(Iter found, Iter end)
	: found(found), end(end) {}

	operator bool() const { return found != end; }
	Iter item() const { return found; }
};

template<typename Container, typename Predicate>
auto find_if(Container&& c, Predicate pred)
{
	auto end = std::end(std::forward<Container>(c));
	auto found = std::find_if(std::begin(std::forward<Container>(c)), end, pred);
	return found_type<decltype(std::begin(std::forward<Container>(c)))>{found, end};
}

template<typename Container, typename KeyType>
auto find(Container&& c, KeyType const& key)
->found_type<decltype(std::begin(std::forward<Container>(c)))>
{
	return {std::forward<Container>(c).find(key), std::forward<Container>(c).end()};
}

//template<typename Container>
//found_type<typename Container::const_iterator> find(Container const& c, typename Container::key_type const& key)
//{
//	return {c.find(key), c.end()};
//}

class parsing_error
: public std::runtime_error
{
public:
	parsing_error(std::string const& msg, unsigned line)
	: std::runtime_error(msg + " at line " + std::to_string(line)) {}
};

class access_error
: public std::runtime_error
{
public:
	access_error(std::string const& msg)
	: std::runtime_error(msg) {}
};

#define throw_parsing_error(msg, line) do{ \
	std::ostringstream oss; \
	oss << __FILE__ << ":" << __LINE__ << ":1 error: " << msg; \
	throw parsing_error(oss.str(), line);}while(0)

#define throw_access_error(msg) do{ \
	std::ostringstream oss; \
	oss << __FILE__ << ":" << __LINE__ << ":1 error: " << msg; \
	throw access_error(oss.str());}while(0)

template<typename T>
struct type_reader
{
	static auto& read(std::istream& is, T& t) { return is >> t; }
};

template<>
struct type_reader<std::string>
{
	static auto& read(std::istream& is, std::string& t) { return std::getline(is, t); }
};

struct opt
{
	uint8_t optional : 1; // 1 -> optional
	uint8_t group_num: 3; // 1-9
	uint8_t group_opp: 2; enum { IOR, XOR, AND };
	uint8_t unique   : 1; // 1 -> unique

	std::string param; // regex
	std::string help;

	mutable bool used; // used in args?
};


// [?!][&^|]?[1-9]?[+]?
// [?!]   - optional/mandatory (required)
// [&^|]? - AND,XOR,IOR type group (optional, dflt: IOR [|])
// [1-9]? - group number (optional, dflt: 0)
// [-+]?   - unique/not-unique? (optional, dflt: unique [-])

// ?&1 -oa1 | // optional AND group #1
// ?&1  -oa2 | if one present, all must be present
// ?&1  -oa3 |
// !&1  -ma1 | // mandatory AND group #1
// !&1  -ma2 | unconditionally all must be present
// !&1  -ma3 |
// ?^1  -ox1 | // optional XOR group #1
// ?^1  -ox2 | only one may be present
// ?^1  -ox3 |
// !^1  -mx1 | mandatory XOR group #1
// !^1  -mx2 | only one must be present
// !^1  -mx3 |
// ?|1  -oi1 | optional IOR group #1
// ?|1  -oi2 | none or all may be present
// ?|1  -oi3 |
// !|1  -mi1 | mandatory IOR group #1
// !|1  -mi2 | at least one must be present
// !|1  -mi3 |

class arg_parser
{
	std::map<std::string, opt> opts;
//	std::vector<val> vals;
	std::multimap<std::string, std::string> vals; // name -> arg
	std::vector<std::string> args;

	void clear()
	{
		decltype(vals)().swap(vals);
		decltype(args)().swap(args);
	}

	static bool is_short(std::string const& name)
	{
		if(name.size() > 1)
			return name[1] != '-';
		return true;
	}

	void construct_options(std::istream& is)
	{
		/**
		 * 1. ? or !
		 * 2. ^3
		 * 3. short|long
		 * 4. short|long
		 * 5. type
		 * 6. help
		 */
		const std::regex e_args
		(
			R"~(^)~"
			R"~(([?!]))~"                  // #1 optional/mandatory
			R"~(([|&^][1-9]?)?)~"          // #2 group-info (default |0)
			R"~(([-+])?)~"                 // #3 uniqueness "+" -> not unique
			R"~(\s+([-][-]?[-\w]+))~"      // #4 mandatory option
			R"~((?:\s+([-][-]?[-\w]+))?)~" // #5 optional second option
			R"~((?:\s+<([^>]+)>)?)~"       // #6 optional param
			R"~((?:\s*[|](.*))?)~"         // #7 optional help
			R"~($)~"
			, std::regex_constants::optimize
		);

		enum OPT {opp = 0, num = 1, optional = 1, group, unique, opt1, opt2, param, help};

		std::smatch m;
		std::string line;
		unsigned line_number = -1;

		hol::trim_mutator trim_mute;

		// test argv data
		std::vector<std::string> argvs;

		while(std::getline(is, line))
		{
			++line_number;

			if(auto pos = line.find("//") + 1)
				line.erase(pos - 1);

			// testing (first line may be argv)
			if(!line.find("argv:"))
			{
				argvs.emplace_back(line.begin() + 5, line.end());
				continue;
			}

			if(trim_mute(line).empty())
				continue;

			if(!std::regex_match(line, m, e_args))
				throw_parsing_error("bad args format (" + line + ")", line_number);

//			for(auto&& s: m)
//				bug_var(s.str());
//			bug("");

			opt o;
			o.optional  = m.str(OPT::optional)[0] == '?';
			o.group_opp = opt::IOR;
			o.group_num = 0;
			if(!m.str(OPT::group).empty())
			{
				o.group_opp =
					m.str(OPT::group)[OPT::opp] == '|' ? opt::IOR
					: m.str(OPT::group)[OPT::opp] == '&' ? opt::AND
					: opt::XOR;

				if(m.str(OPT::group).size() > 1) // optional group_num (1-9)
					o.group_num = m.str(OPT::group)[OPT::num] - '0';
			}
			o.unique = 1;
			if(!m.str(OPT::unique).empty())
				o.unique = m.str(OPT::unique)[0] != '+';

			o.param = m.str(OPT::param);
			o.help = m.str(OPT::help);
			trim_mute(o.help);

			if(!m.str(OPT::opt1).empty())
				opts[m.str(OPT::opt1)] = o;

			if(!m.str(OPT::opt2).empty())
				opts[m.str(OPT::opt2)] = o;
		}

		if(!argvs.empty())
		{
			// found testing data - run tests

			hol::split_copier split_copy;
			split_copy.fold_matches();

			dump(std::cout);

			// testing
			for(auto const& argv: argvs)
			{
				auto args = split_copy.at_space(argv);

				std::vector<const char*> v;
				for(auto const& s: args)
					v.push_back(s.c_str());
				v.push_back(nullptr);

				std::cout << "Parsing: " << argv << '\n';
				clear();
				parse(v.data());
				std::cout << '\n';
			}
		}
	}

public:
	arg_parser(std::istream& options) { construct_options(options); }
	arg_parser(std::istream&& options): arg_parser(options) {}
	arg_parser(std::string const& options): arg_parser(std::istringstream(options)) {}

	void dump(std::ostream& os)
	{
		std::map<int, char> oppmap {{opt::AND, '&'}, {opt::IOR, '|'}, {opt::XOR, '^'}};

		for(auto const& o: opts)
		{
			os << o.first;
			os << ": " << (o.second.optional ? '?':'!');
			os << " " << oppmap.at(o.second.group_opp);
			os << int(o.second.group_num);
			os << " " << (o.second.unique ? '-':'+');
			os << ' ' << (is_short(o.first)?'S':'L');
			if(!o.second.param.empty())
				os << ' ' << o.second.param;
			os << ' ' << o.second.help;
			os << '\n';
		}
	}

	void help(char const*)// arg)
	{
//		if(arg) // keyword help
	}

	template<typename T>
	T convert(std::string const& s)
	{
		std::istringstream iss(s);
		T v {};
		if(!type_reader<T>::read(iss, v))
			throw_access_error("Cannot retrieve value of this type");
		return v;
	}

	template<typename T>
	T get(std::string const& key)
	{
		auto vec = get_vec<T>(key);

		if(vec.empty())
			throw_access_error("No such argument: '" + key + "'");

		return vec[0];
	}

	template<typename T>
	auto get_vec(std::string const& key)
	{
		std::vector<T> vec;

		auto found = vals.equal_range(key);

		for(; found.first != found.second; ++found.first)
			vec.push_back(convert<T>(found.first->second));

		return vec;
	}

	void parse(char const* const argv[])
	{
		using namespace hol::small_types::string_containers;

		// Chech for --help -h special case
		// if found dump help
		// otherwise build vals
		if(argv[1]
		&& (!std::strcmp(argv[1], "--help")
		||  !std::strcmp(argv[1], "-h")))
			help(argv[2]);
		else for(auto arg = argv; *arg; ++arg)
		{
			if(auto found = find(opts, *arg))
			{
				// -s <int>
				found.item()->second.used = true;

				std::string key = found.item()->first;
				std::string val;

				const std::map<std::string, std::regex> regs
				{
					  {"int", std::regex("\\d+", std::regex_constants::optimize)}
					, {"real", std::regex("\\d*\\.\\d+", std::regex_constants::optimize)}
					, {"float", std::regex("\\d*\\.\\d+", std::regex_constants::optimize)}
					, {"string", std::regex(".*", std::regex_constants::optimize)}
				};

				if(!found.item()->second.param.empty())
				{
					auto param = found.item()->second.param;

					// have parameter, needs type checking
					if(!*++arg)
						throw_access_error(std::string(*(arg - 1))
							+ " expects a parameter: " + param);

					std::regex e(param);
					if(auto found = find(regs, param))
						e = found.item()->second;

					if(!std::regex_match(*arg, e))
						throw_access_error(std::string(*(arg - 1))
							+ " bad argument, expecting "
								+ param + " got " + *arg);

					val = *arg;
				}
				vals.emplace(key, val);
//				vals.push_back(v);
			}
			else
			{
				args.push_back(*arg);
			}
		}

		auto end = std::end(opts);

		// now check rules
		for(auto o = std::begin(opts); o != end; ++o)
		{
			// do we have one on the cmd line?
			auto found = vals.equal_range(o->first);

			// check mandatories
			if(!o->second.optional)
				if(found.first == found.second)
					throw_access_error("Missing mandatory option: " << o->first);


			// here the item is either optional XOR mandatory and present

			// is it a member of the group (optional member)
			if(found.first != found.second)
			{
				// check group logic
				if(o->second.group_opp == opt::AND)
					check_and_group(o, end);
//				else if(o->second.group_opp == opt::IOR)
//					check_ior_group(o, end);
				else if(o->second.group_opp == opt::XOR)
					check_xor_group(o, end);
			}
		}
	}

	template<typename Test>
	void check_group(decltype(opts)::const_iterator o, decltype(o) end, Test test)
	{
		for(auto begin = o; o != end; ++o)
		{
			if(o->second.optional
			|| o->second.group_num != begin->second.group_num
			|| o->second.group_opp != opt::AND)
				continue;

			// do we have one on the cmd line?
			auto found = vals.equal_range(o->first);

			test(found);
		}
	}

	void check_and_group(decltype(opts)::const_iterator o, decltype(o) end)
	{
		check_group(o, end, [o](auto found)
		{
			if(found.first == found.second)
				throw_access_error("Required group option not present: &" << o->second.group_num << " " << o->first);
		});
	}

//	void check_ior_group(decltype(opts)::const_iterator o, decltype(o) end)
//	{
//	}

	void check_xor_group(decltype(opts)::const_iterator o, decltype(o) end)
	{
		check_group(o, end, [o](auto found)
		{
			if(found.first != found.second)
				throw_access_error("Only one option allowed in this group: ^" << o->second.group_num << " " << o->first);
		});
	}
};

}} // namespace hol::arg

#endif // HOL_CLI_ARGS_H
