#ifndef WFC_UTILS_H_
#define WFC_UTILS_H_

#include <cstdlib>
#include <iterator>
#include <unordered_set>

namespace wfc {

/*
 * Find intersection of two sets
 */
template<typename S>
std::unordered_set<S> set_intersection(const std::unordered_set<S>& set1, const std::unordered_set<S>& set2) {
  const std::unordered_set<S>& small = (set1.size() < set2.size()) ? set1 : set2;
  const std::unordered_set<S>& large = (set1.size() < set2.size()) ? set2 : set1;

  std::unordered_set<S> result;
  for (const S& elem : small) {
    if (large.count(elem)) {
      result.insert(elem);
    }
  }
  return result;
}

/*
 * Find difference of one set from other
 */
template<typename S>
std::unordered_set<S> set_difference(const std::unordered_set<S>& set1, const std::unordered_set<S>& set2) {
  std::unordered_set<S> result;
  for (const S& elem : set1) {
    if (set2.count(elem) == 0) {
      result.insert(elem);
    }
  }

  return result;
}

template<typename S>
auto select_from(const S &s, size_t n) {
  auto it = std::begin(s);
  // 'advance' the iterator n times
  std::advance(it,n);
  return it;
}

}

#endif // !WFC_UTILS_H_
