// Generalize a function with templates
//
// Step 1: implement sum for a std::vector<int>
// Step 2: turn it into a function template working with a std::vector of any
//         element type
// Step 3: generalize it further, so that it works with any container
//         (hint: the type of the elements is available as C::value_type)

#include <array>
#include <cassert>
#include <list>
#include <set>
#include <vector>

// Step 1
int sum(const std::vector<int>& v) {
  // TODO
  return 0;
}

// Step 2
// TODO

// Step 3
// TODO

int main() {
  assert(sum(std::vector<int>{1, 2, 3}) == 6);

  assert(sum(std::vector<double>{0.5, 1.5}) == 2.);
  assert(sum(std::vector<long>{}) == 0);

  assert(sum(std::list<int>{1, 2, 3}) == 6);
  assert(sum(std::set<float>{1.f, 2.f}) == 3.f);
  assert((sum(std::array<long, 3>{1, 2, 3}) == 6));
}
