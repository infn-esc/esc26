// Compose concepts
//
// 1. Define the concept Number, satisfied by integral and floating point types
// 2. Implement average, returning the mean of the elements of a container
// 3. Constrain average so that it accepts only containers of numbers

#include <array>
#include <cassert>
#include <concepts>
#include <list>
#include <string>
#include <vector>

// Same as in the slides
template <typename T>
concept Container = requires(T t) {
  typename T::value_type;
  typename T::size_type;
  typename T::iterator;
  typename T::const_iterator;
  t.size();
  t.begin();
  t.end();
  t.cbegin();
  t.cend();
};

template <typename T>
concept Number = ;  // TODO

template <typename C>  // TODO: add the constraints
double average(const C& c) {
  // TODO
  return 0.;
}

template <typename C>
concept Averageable = requires(const C& c) { average(c); };

static_assert(Number<int>);
static_assert(Number<unsigned long>);
static_assert(Number<double>);
static_assert(!Number<int*>);
static_assert(!Number<std::string>);

static_assert(Averageable<std::vector<int>>);
static_assert(Averageable<std::list<double>>);
static_assert(Averageable<std::array<float, 3>>);
static_assert(!Averageable<int>);
static_assert(!Averageable<std::vector<std::string>>);

int main() {
  assert(average(std::vector<int>{1, 2, 3, 4}) == 2.5);
  assert(average(std::list<double>{0.5, 1.5}) == 1.);
  assert((average(std::array<float, 3>{1.f, 2.f, 3.f}) == 2.));
}
