// Define your own standard type traits

#include <list>
#include <type_traits>
#include <vector>

// 1. my_is_same<T, U>: true if T and U are the same type
template <typename T, typename U>
struct my_is_same : std::false_type {};

// TODO: specialization

template <typename T, typename U>
constexpr bool my_is_same_v = my_is_same<T, U>::value;

// 2. my_remove_const<T>: removes the top-level const from T
template <typename T>
struct my_remove_const {
  using type = T;
};

// TODO: specialization

template <typename T>
using my_remove_const_t = typename my_remove_const<T>::type;

// 3. is_vector<T>: true if T is a std::vector
//    (hint: how many template parameters does std::vector have?)
template <typename T>
struct is_vector : std::false_type {};

// TODO: specialization

template <typename T>
constexpr bool is_vector_v = is_vector<T>::value;

static_assert(my_is_same_v<int, int>);
static_assert(!my_is_same_v<int, float>);
static_assert(!my_is_same_v<int, const int>);
static_assert(!my_is_same_v<int, int&>);

static_assert(my_is_same_v<my_remove_const_t<int>, int>);
static_assert(my_is_same_v<my_remove_const_t<const int>, int>);
static_assert(my_is_same_v<my_remove_const_t<int* const>, int*>);
static_assert(my_is_same_v<my_remove_const_t<const int*>, const int*>);  // why?

static_assert(is_vector_v<std::vector<int>>);
static_assert(is_vector_v<std::vector<std::vector<float>>>);
static_assert(!is_vector_v<std::list<int>>);
static_assert(!is_vector_v<int>);

template <typename T>
struct MyAllocator : std::allocator<T> {};
static_assert(is_vector_v<std::vector<int, MyAllocator<int>>>);

int main() {}
