#ifndef DISJOINT_SET_H
#define DISJOINT_SET_H

#include <map>

namespace DisjointSet 
{
	using std::map;
	using std::swap;
	using ll = long long;

	class DisjointSet
	{
	public:
		DisjointSet() {};
		bool add(ll id);
		bool erase(ll id);
		ll find(ll id);
		bool merge(ll a, ll b);
		inline int count() const { return parent.size(); };
	private:
		map<ll,ll> parent, size;
	};

	bool DisjointSet::add(ll id) 
	{
		if (not parent.count(id)) {
			size[id] = 1;
			parent[id] = id;
			return true;
		}
		return false;
	}

	bool DisjointSet::erase(ll id)
	{
		auto iter = parent.find(id);
		if (iter != parent.end()) {
			parent.erase(iter);
			size.erase(id);
			return true;
		}
		return false;
	}

	ll DisjointSet::find(ll id)
	{
		ll& p = parent[id];
		if (p != id) p = find(p);
		return p;
	}

	bool DisjointSet::merge(ll a, ll b)
	{
		a = find(a), b = find(b);
		if (a != b) {
			ll size_a = size[a], size_b = size[b];
			if (size_a < size_b) swap(a, b); // force size[a] >= size[b]
			parent[b] = parent[a];
			size[a] = size_a + size_b;
			erase(b);
			return true;
		}
		return false;
	}

}

#endif