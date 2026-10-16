// Define the three concepts below, each one built on top of the
// previous one:
//  - Container: has the nested types value_type, iterator and
//    const_iterator, and the methods begin() and end()
//  - SizedContainer: a Container with a size() method returning
//    something convertible to std::size_t
//  - RandomAccessContainer: a SizedContainer whose elements can be
//    accessed with c[i]
//  The static_asserts check your definitions: the file compiles only
//  when the concepts are correct.

#include <array>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <vector>

template <typename T>
concept Container = ;  // TODO

template <typename T>
concept SizedContainer = ;  // TODO

template <typename T>
concept RandomAccessContainer = ;  // TODO

std::string_view kind(Container auto const&) { return "container"; }
std::string_view kind(SizedContainer auto const&) { return "sized container"; }
std::string_view kind(RandomAccessContainer auto const&) { return "random access container"; }

static_assert(RandomAccessContainer<std::vector<int>>);
static_assert(RandomAccessContainer<std::deque<int>>);
static_assert(RandomAccessContainer<std::array<int, 3>>);
static_assert(RandomAccessContainer<std::string>);

static_assert(SizedContainer<std::list<int>>);
static_assert(SizedContainer<std::set<int>>);
static_assert(!RandomAccessContainer<std::list<int>>);
static_assert(!RandomAccessContainer<std::set<int>>);

static_assert(Container<std::forward_list<int>>);
static_assert(!SizedContainer<std::forward_list<int>>);

static_assert(!Container<int>);
static_assert(!Container<int*>);
static_assert(!Container<int[3]>);

int main() {
  assert(kind(std::vector<int>{}) == "random access container");
  assert(kind(std::string{}) == "random access container");
  assert(kind(std::list<int>{}) == "sized container");
  assert(kind(std::forward_list<int>{}) == "container");
}
