#ifndef HEADER_ONLY_LIBRARY_SPAN_ALLOCATOR_H
#define HEADER_ONLY_LIBRARY_SPAN_ALLOCATOR_H

#include <optional>
#include <vector>

#include <gsl/span>

namespace header_only_library {
namespace containers {

template<typename T>
class span_allocator
{
	using type = T;
	using span = gsl::span<type>;

public:
	span_allocator(std::size_t n)
	: m_data(n), m_free(1, span{m_data.data(), gsl::index(m_data.size())}) {}

	void resize(std::size_t n)
	{
		m_data.clear();
		m_used.clear();
		m_free.clear();
		m_data.resize(n);
	}

	// TODO: Make this policy plugable
	std::optional<span> allocate(gsl::index n)
	{
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
		bug_cnt(m_data);

		bug("m_used:");
		for(auto sp: m_used)
			std::cout << '{' << std::distance(m_data.data(), sp.data())
				<< ", " << std::distance(m_data.data(), sp.data() + sp.size()) << '}' << '\n';

		bug("m_free:");
		for(auto sp: m_free)
			std::cout << '{' << std::distance(m_data.data(), sp.data())
				<< ", " << std::distance(m_data.data(), sp.data() + sp.size()) << '}' << '\n';
	}

private:
	void move_to_free(typename std::vector<span>::iterator found)
	{
		if(!m_auto_defrag)
			m_free.push_back(*found);
		else
		{
			auto lb = std::lower_bound(std::begin(m_free), std::end(m_free), *found);

			if(lb != std::end(m_free) && found->data() + found->size() == lb->data())
				*lb = gsl::make_span(found->data(), found->size() + lb->size());
			else
				m_free.insert(lb, *found);
		}

		m_used.erase(found);
	}

	bool m_auto_defrag = false; // if true, deallocate => OlogN, else O1
	std::vector<type> m_data;
	std::vector<span> m_used;
	std::vector<span> m_free;
};

} // namespace containers
} // namespace header_only_library

#endif // HEADER_ONLY_LIBRARY_SPAN_ALLOCATOR_H

