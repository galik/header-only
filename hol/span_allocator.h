#ifndef HEADER_ONLY_LIBRARY_SPAN_ALLOCATOR_H
#define HEADER_ONLY_LIBRARY_SPAN_ALLOCATOR_H

#include <algorithm>
#include <cassert>
#include <optional>
#include <vector>

#include <gsl/span>

namespace header_only_library {
namespace containers {

template<typename T>
class span_allocator
{
	template<typename TT>
	class vector
	: public std::vector<TT>
	{
	public:
		using std::vector<TT>::vector;
		gsl::index size() const { return gsl::index(std::vector<TT>::size()); }

	};

public:
	using value_type = T;
	using span = gsl::span<value_type>;

	span_allocator(std::size_t n)
	: m_data(n), m_free(1, span{m_data.data(), gsl::index(m_data.size())}) {}

	void resize(std::size_t n)
	{
		m_data.clear();
		m_used.clear();
		m_free.clear();
		m_data.resize(n);
	}

	std::vector<span> divide_into(gsl::index n)
	{
		m_used.clear();
		m_free.clear();
		std::vector<span> sps;

		auto sz = (m_data.size() / n) + (m_data.size() % n);

		while(auto sp = allocate(sz))
			sps.push_back(sp.value());

		return sps;
	}

	// TODO: Make this policy plugable
	std::optional<span> allocate(gsl::index n)
	{
		assert(n > 0);
		assert(n <= gsl::index(m_data.size()));

		auto found = std::find_if(std::begin(m_free), std::end(m_free), [n](span s){
			return n == s.size();
		});

		if(found != std::end(m_free))
		{
			m_used.push_back(*found);
			m_free.erase(found);
			return m_used.back();
		}

		found = std::find_if(std::begin(m_free), std::end(m_free), [n](span s){
			return n < s.size();
		});

		if(found == std::end(m_free))
			return {};

		m_used.push_back(found->subspan(0, n));
		*found = found->subspan(n);
		return m_used.back();
	}

	void deallocate(span s)
	{
		auto found = std::find_if(std::begin(m_used), std::end(m_used), [s](span s1){
			return s.data() == s1.data();
		});

		if(found == std::end(m_used))
			throw std::runtime_error("span was not allocated here");

		move_to_free(found);
	}

	void defragment_automatically() { m_auto_defrag = true; }
	void do_not_defragment_automatically() { m_auto_defrag = false; }

	void defragment()
	{
		if(m_auto_defrag || m_free.empty())
			return;

		std::sort(std::begin(m_free), std::end(m_free), [](span a, span b){
			return a.data() < b.data();
		});

		for(auto s = std::begin(m_free); s != std::prev(std::end(m_free)); ++s)
		{
			if(s->data() + s->size() != std::next(s)->data())
				continue;

			*std::next(s) = gsl::make_span(s->data(), s->size() + std::next(s)->size());
			*s = gsl::make_span(s->data(), s->data());
		}

		m_free.erase(std::remove_if(std::begin(m_free), std::end(m_free), [](span s){
			return s.empty();
		}), std::end(m_free));
	}

	void dump()
	{
		std::cout << "m_data: " << m_data.size() << '\n';
		int i = 0;
		for(auto sp: m_data)
			std::cout << i++ << ": " << sp << '\n';

		std::cout << "m_used: " << m_used.size() << '\n';
		for(auto sp: m_used)
			print(sp);

		std::cout << "m_free: " << m_free.size() << '\n';
		for(auto sp: m_free)
			print(sp);
	}

	std::vector<gsl::index> state()
	{
		std::vector<gsl::index> s;
		// used_size, {used pos}*, free_size, {free pos}*
		{
			auto temp = m_used;
//			std::sort(std::begin(temp), std::end(temp));
			s.push_back(temp.size());
			std::transform(std::begin(temp), std::end(temp), std::back_inserter(s), [this](span sp){
				return std::distance(m_data.data(), sp.data());
			});
		}
		{
			auto temp = m_free;
//			std::sort(std::begin(temp), std::end(temp));
			s.push_back(temp.size());
			std::transform(std::begin(temp), std::end(temp), std::back_inserter(s), [this](span sp){
				return std::distance(m_data.data(), sp.data());
			});
		}
		return s;
	}

private:

	void print(span sp)
	{
		std::cout << '{' << std::distance(m_data.data(), sp.data())
			<< ", " << std::distance(m_data.data(), sp.data() + sp.size()) << '}' << '\n';
	}

	void move_to_free(typename std::vector<span>::iterator found)
	{
		bug_fun();
//		bug_var(m_auto_defrag);
//		bug("============");
		print(*found);
//		bug("============");

		if(!m_auto_defrag)
			m_free.push_back(*found);
		else
		{
			auto lb = std::lower_bound(std::begin(m_free), std::end(m_free), *found);

			if(lb == std::end(m_free))
				lb = m_free.insert(lb, *found);
			else
			{
				if(found->data() + found->size() == lb->data())
					*lb = gsl::make_span(found->data(), found->size() + lb->size());
				else
					lb = m_free.insert(lb, *found);
			}

			if(lb != std::begin(m_free))
			{
				auto prev = std::prev(lb);

				if(prev->data() + prev->size() == lb->data())
				{
					*prev = gsl::make_span(prev->data(), prev->size() + lb->size());
					lb = m_free.erase(lb);
				}
			}
		}

		m_used.erase(found);
	}

	bool m_auto_defrag = false; // if true, deallocate => OlogN, else O1
	vector<value_type> m_data;
	vector<span> m_used;
	vector<span> m_free;
};

} // namespace containers
} // namespace header_only_library

#endif // HEADER_ONLY_LIBRARY_SPAN_ALLOCATOR_H

