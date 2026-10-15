---
title: Atomics and mutex
layout: main
section: memory
---

# Atomics and mutex

The code is in
[`hands-on/memory/atomics_mutex`]({{ site.github.repository_url }}/tree/main/hands-on/memory/atomics_mutex).

## Exercise 1: the counter of the slides

`counter.cc` is the program of the slides: two threads increment the same `int` one million
times each.

```shell
g++ -O0 -pthread counter.cc -o counter
./counter
```

- Run it a few times. Is the result 2,000,000? Why not?
- Compile with `-O2` and run again. Does it work? Do you think your program is correct?  
- Fix it with `std::atomic`, and check the result at `-O0` and `-O2`.

## Exercise 2: producer and consumer

Modify `atomics.cc`. A producer thread fills a vector; a consumer thread reads the vector and
sums its elements, then checks the sum. 

```shell
g++ -O2 -pthread atomics.cc -o atomics
./atomics
```

- Is the sum always correct? Try different optimisation levels.
- Fix it with an atomic flag: the producer sets it when it has finished, and the consumer waits
  for it before reading. Which memory order do the store and the load need? Would a plain
  `bool` work?

## Exercise 3: even and odd numbers with a mutex

Modify `mutex.cc`: count the even and the odd numbers of a vector in parallel, with
`std::thread`.

- Split the vector into one chunk per thread. Make sure the chunks cover the whole vector,
  also when its size is not a multiple of the number of threads.
- Use a `std::mutex` so that the two shared counters are correct. Use `std::lock_guard`
  instead of calling `lock()` and `unlock()` by hand: why?
- How often does each thread need to lock the mutex? Compare locking once per element with
  locking once per thread.
- Then try with `std::atomic<int>` counters instead of the mutex.

```shell
g++ -std=c++20 -Wall -O2 -pthread mutex.cc -o mutex
./mutex
```
