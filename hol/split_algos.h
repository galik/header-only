#ifndef HOL_SPLIT_ALGOS_H
#define HOL_SPLIT_ALGOS_H

#include "macro_exceptions.h"
#include "macro_utils.h"

namespace header_only_library {
namespace algorithm {
namespace chr {

inline char const* space(char) { return " "; }
inline wchar_t const* space(wchar_t) { return L" "; }
inline char16_t const* space(char16_t) { return u" "; }
inline char32_t const* space(char32_t) { return U" "; }

} // namespace chr

using namespace header_only_library::macro_utils;

template<typename SearchIter, typename DelimIter, typename Inserter>
void split(SearchIter beg_s, SearchIter end_s, DelimIter beg_d, DelimIter end_d, Inserter out)
{
	auto size_d = std::distance(beg_d, end_d);

	HOL_ASSERT_MSG(size_d, "delimiter cannot be empty");

	std::size_t count = 0U;
	SearchIter pos;

	while((pos = std::search(beg_s, end_s, beg_d, end_d)) != end_s)
	{
		out.insert(beg_s, pos);
		beg_s = std::next(pos, size_d);
		++count;
	}

	if(count)
		out.insert(beg_s, pos);
}

template<typename SearchIter, typename DelimIter, typename Inserter>
void split_fold(SearchIter beg_s, SearchIter end_s, DelimIter beg_d, DelimIter end_d, Inserter out)
{
	auto size_d = std::distance(beg_d, end_d);

	HOL_ASSERT_MSG(size_d, "delimiter cannot be empty");

	std::size_t count = 0U;
	SearchIter pos;

	while((pos = std::search(beg_s, end_s, beg_d, end_d)) != end_s)
	{
		if(pos - beg_s)
			out.insert(beg_s, pos);
		beg_s = std::next(pos, size_d);
		++count;
	}

	if(count && pos - beg_s)
		out.insert(beg_s, pos);
}

template<typename Container>//, typename Contained>
class emplace_back_inserter
{
public:
	emplace_back_inserter(Container& c): c(c) {}

	template<typename Iter>

	void insert(Iter b, Iter e) { c.emplace_back(b, e); }

private:
	Container& c;
};

template<typename Container>
auto inserter(Container& c)
{
	return emplace_back_inserter<Container>(c);
}

//template<typename Container>
//class emplace_back_span_inserter
//{
//public:
//	emplace_back_span_inserter(Container& c): c(c) {}
//
//	template<typename Iter>
//	void insert(Iter b, Iter e)
//	{
//		if(b != e)
//			c.emplace_back(&*b, std::distance(b, e));
//		else
//			c.emplace_back();
//	}
//
//private:
//	Container& c;
//};
//
//template<typename Container>
//auto span_inserter(Container& c)
//{
//	return emplace_back_span_inserter<Container>(c);
//}

} // namespace algorithm
} // namespace header_only_library

#endif // HOL_SPLIT_ALGOS_H
