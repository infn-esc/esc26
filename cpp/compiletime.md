---
title: Compile-time computation
layout: main
section: cpp
---

The goal of these exercises is to get some feeling about the possibilities
offered by the C++ language to perform computations at compile-time.

## Exercise 1

Starting from [`my_traits.cpp`]({% include hands_on_base.md %}/cpp/my_traits.cpp), implement your own type traits by writing the missing template specializations:

- `my_is_same<T, U>`: true if `T` and `U` are the same type, like `std::is_same`;
- `my_remove_const<T>`: removes the top-level `const` from `T`, like `std::remove_const`;
- `is_vector<T>`: true if `T` is a `std::vector` (this one has no equivalent in the standard library).

The `static_assert`s in the file check your implementation: the file compiles only when the traits are correct.

## Exercise 2

Starting from [`sum_generic.cpp`]({% include hands_on_base.md %}/cpp/sum_generic.cpp), generalize the sum of the elements of a container to any container.

## Exercise 3

Starting from [`average_concepts.cpp`]({% include hands_on_base.md %}/cpp/average_concepts.cpp), implement a `Number` concept representing both integer and floating point numbers and write a function computing the average of a container of `Number`s.

## Exercise 4

Starting from [`container_concepts.cpp`]({% include hands_on_base.md %}/cpp/container_concepts.cpp), write concepts representing generic containers, sized containers (i.e. containers with a `size` method) and random access containers (i.e. sized containers implementing `operator[]`).

## Exercise 5

Starting from [`iterator_concepts.cpp`]({% include hands_on_base.md %}/cpp/iterator_concepts.cpp), implement concepts for forward iterators and random access iterators, then write a function computing the middle iterator for a pair of iterators, overloaded for both iterator types.

Hint: a random access iterator is also a forward iterator, so both overloads of `middle` are viable for a `std::vector` iterator. Think about how to define the two concepts so that the compiler can tell which one is more specific and pick that overload, instead of reporting an ambiguous call.

## Exercise 6

Take the `pi` function in [`pi_time.cpp`]({% include hands_on_base.md
%}/basic/pi_time.cpp) and make it `constexpr`

Look at the assembly (the easiest is to copy&paste the code into [Compiler
Explorer](https://godbolt.org/)) and verify that there is no computation
performed at runtime.

## Exercise 7

Implement a `constexpr` function that checks if a number is prime.

Like before, look at the assembly and verify that there is no computation
performed at runtime for a value known _statically_ (i.e. known at compile
time): the value can be either a literal or the result of another `constexpr`
computation.

Check that it's possible to call the function with a value not known statically,
e.g. one that is asked on the standard input.

## Exercise 8

Take [`containers_assoc.cpp`]({% include hands_on_base.md %}/{{ page.section
}}/containers_assoc.cpp) and extend it to cover also the use of the `std::set`
and `std::unordered_set` associative containers. To fill the associative
containers you can simply insert all the numbers from 0 to N, without random
generation and without advancing.

In order to dispatch to the correct implementation you can use the
`is_associative` trait already included in that file, using it either as a tag
or in a _constexpr-if_.

## Exercise 9

Construct a compile-time table corresponding to a [Pascal’s
Triangle](https://en.wikipedia.org/wiki/Pascal%27s_triangle) of N rows, where N
is a compile-time constant.

Look at the assembly and verify that there is no computation performed at
runtime for a statically-known value of a certain position, but that it's also
possible to access the value for a position obtained at runtime, e.g. from
standard input.
