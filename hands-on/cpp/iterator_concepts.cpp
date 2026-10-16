// Concepts for iterators and overloading
// Define concepts for forward iterators and random access iterators.
// Then, define functions taking a pair of iterators and returning an iterator
// pointing to the middle of the container, overloading for the two different
// kinds of iterators.
// The middle is the element at position size / 2, e.g. 3 for {1, 2, 3, 4}.
//
// Hint: remember that Forward iterator only implements operator++, while
// random access iterator also implements operator+=, operator-- and operator-=,
// operator+ and operator-.

#include <cassert>
#include <concepts>
#include <forward_list>
#include <iterator>
#include <list>
#include <vector>

// Part 1
template <typename It>
concept ForwardIterator = ;  // TODO

template <typename It>
concept RandomAccessIterator = ;  // TODO

// Part 2
template <ForwardIterator It>
It middle(It first, It last) {
  // TODO
  return first;
}

template <RandomAccessIterator It>
It middle(It first, It last) {
  // TODO
  return first;
}

static_assert(RandomAccessIterator<std::vector<int>::iterator>);
static_assert(RandomAccessIterator<std::vector<int>::const_iterator>);
static_assert(RandomAccessIterator<int*>);

static_assert(ForwardIterator<std::list<int>::iterator>);
static_assert(ForwardIterator<std::forward_list<int>::iterator>);
static_assert(!RandomAccessIterator<std::list<int>::iterator>);
static_assert(!RandomAccessIterator<std::forward_list<int>::iterator>);

static_assert(!ForwardIterator<int>);
static_assert(!ForwardIterator<std::vector<int>>);

int main() {
  std::vector<int> v{1, 2, 3, 4, 5};
  std::list<int> l{1, 2, 3, 4, 5};
  assert(*middle(v.begin(), v.end()) == 3);
  assert(*middle(l.begin(), l.end()) == 3);

  std::vector<int> v_even{1, 2, 3, 4};
  std::list<int> l_even{1, 2, 3, 4};
  assert(*middle(v_even.begin(), v_even.end()) == 3);
  assert(*middle(l_even.begin(), l_even.end()) == 3);

  std::forward_list<int> fl{1, 2, 3};
  assert(*middle(fl.begin(), fl.end()) == 2);

  std::vector<int> empty;
  assert(middle(empty.begin(), empty.end()) == empty.end());
}
