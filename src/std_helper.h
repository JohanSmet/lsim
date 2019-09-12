// std_helper.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// some quality-of-life improvements when working with the C++ STL

#ifndef LSIM_STD_HELPER_H
#define LSIM_STD_HELPER_H

#include <algorithm>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace lsim {

// repeating std:: gets tedious and unreadable quickly in some case 
using std::begin;
using std::end;
using std::move;
using std::string;

// functions to streamline removing elements from a container
template<typename T>
void remove(std::vector<T>& container, const T& value) {
	container.erase(std::remove(begin(container), end(container), value), end(container));
}

template<typename T>
void remove_owner(std::vector<std::unique_ptr<T>>& container, const T* value) {
	container.erase(std::remove_if(begin(container), end(container), [value](const auto& el) {return el.get() == value; }),
					end(container));
}

template<typename K, typename V> 
void remove_value(std::unordered_map<K, V>& map, const V& value) {
	for (auto iter = begin(map); iter != end(map);) {
		if (iter->second == value) {
			iter = map.erase(iter);
		}
		else {
			++iter;
		}
	}
}

template <typename T, typename Predicate>
void remove_if(std::vector<T>& container, Predicate condition) {
	container.erase(std::remove_if(begin(container), end(container), condition), end(container));
}


} // namespace lsim

#endif // LSIM_STD_HELPER_H

