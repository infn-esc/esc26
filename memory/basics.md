---
title: Basics
layout: main
section: memory
---

# Basics

## Exercise 1: sizeof and alignof

Write a small program that prints the size (`sizeof`) and the alignment (`alignof`) of these types:

- `char`, `bool`, `int`, `float`, `double`, `void*`
- `std::vector<double>` and `std::string`

You can also do this in [Compiler Explorer](https://godbolt.org/z/KM78oxYcY).

## Exercise 2: member order

Write a struct with a `char`, a `double` and an `int`, in this order, and print its size and
alignment. Then change the order of the members.

- Does the size change? Try playing around with the orders 
