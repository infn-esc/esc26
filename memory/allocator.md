---
title: Allocating memory
layout: main
section: memory
---

# Allocating memory

The code is in
[`hands-on/memory/allocators`]({{ site.github.repository_url }}/tree/main/hands-on/memory/allocators).

## Exercise 1: a single allocation 
don't spend too much time here, feel free to look at the solution after a while

Modify `single_bufferSoA.cc`. Write a different version of the SoA particle:

- every column (member) is a **pointer**, not a `std::vector`;
- the constructor takes the number of particles `N`;
- instead of allocating each column separately, the constructor allocates **all** the memory
  needed in **one** allocation, and keeps a pointer to it in an extra member, the buffer
  (for example `char* buffer`);
- then it points each column at the right place inside the buffer;
- the destructor frees the buffer.

```shell
g++ -std=c++20 -Wall -g single_bufferSoA.cc -o single_bufferSoA
./single_bufferSoA
```

- How many bytes do you need in total? Count them column by column.
- Print the address of each column and check the distances between them.
- Compile with `-fsanitize=address` and run again: does AddressSanitizer find any error?

## Exercise 2: a caching allocator

Look at the slides on how a caching allocator works. Then go to `caching_allocator/` and
modify `CachingAllocator.h`. The class `CachingAllocator` has three methods:

- `allocate(std::size_t size)`: returns a block of at least `size` bytes. If a free block of
  the right size is cached, return it; otherwise allocate a new one.
- `deallocate(void* ptr)`: does **not** free the memory. It puts the block back in the cache,
  so that the next `allocate` can reuse it.
- `free()`: releases all the memory, the blocks in use and the cached ones.

The blocks come in a few fixed sizes (bins), each 8 times the previous one: 512 B, 4 KiB,
32 KiB, ... A request gets the smallest bin that is big enough. Requests bigger than the largest
bin are not cached.

```shell
cd caching_allocator
g++ -std=c++20 -Wall -O2 main.cc -o caching_allocator
./caching_allocator
```

`main.cc` first runs a few allocations with the debug printout on, so that you can see what
your allocator does. Then it runs "events" that allocate three arrays, use them and free them,
and compares your allocator with `malloc`/`free` and with `std::vector`. It prints the time and
the number of page faults of each.

- Start with the debug printout: is a block reused when it should be?
- Compare the small and the large arrays. Where does the caching allocator help, and where
  does it not? Why? 
- Why is `std::vector<double>(N)` slower than `malloc`?
