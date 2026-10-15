---
title: Data structures
layout: main
section: memory
---

# Data structures

The code is in
[`hands-on/memory/datastructures`]({{ site.github.repository_url }}/tree/main/hands-on/memory/datastructures).

## Exercise 1: the particle struct

Modify `Particle.h`: give the struct `GoodParticle` these members:

- one `std::string` for the particle's name;
- three `double`s for the x, y, z coordinates;
- three `bool`s to mark a collision along the x, y, z directions;
- one `float` for the mass;
- one `float` for the energy;
- three `double`s for the px, py, pz momenta;
- one `int` for the particle's id.

What is the best order for the members?

Compile and run `size_of_particle.cc` to check the size of your `GoodParticle`:

```shell
g++ -std=c++20 -Wall -O2 size_of_particle.cc -o size_of_particle
./size_of_particle
```

- What is the smallest size you can get? 
- How many cache lines does one particle take?

## Exercise 2: the access pattern

Now use `GoodParticle` for a computation, in `aos_soa.cc`.

- Write a function that initialises a collection of `N` `GoodParticle`s, with every member
  set. `main` already generates the random values; every particle starts inside the box,
  between 0 and `x_max`.
- Write a function that takes the collection, a time interval `t` and `x_max`. For every
  particle:
  - update the position: `x = x + px / m * t`;
  - if the new `x` is below 0 or above `x_max`, set `hit_x` to true and change the sign of `px`;
  - otherwise set `hit_x` to false.

  Repeat this `NIter` times (`t` is the same for all particles and iterations).

How are you accessing the members of the data structure? Of all the bytes of a particle, how
many does this loop use?

## Exercise 3: AoS vs SoA

Modify `Particle.h`: write the class `ParticleSoA`, with the same members as `GoodParticle`
but one `std::vector` per member (for example `std::vector<double> x_`).

- Allocate all the vectors for `N` particles once, in the constructor, so that the computation
  never has to resize them.
- Write the two functions of Exercise 2 for the SoA.
- Measure the time of the AoS and of the SoA version and compare them.

```shell
g++ -std=c++20 -Wall -O2 aos_soa.cc -o aos_soa
./aos_soa
```

- Compile with `-O0`, `-O1`, `-O2` and `-O3`. What happens?
- How many bytes does each iteration read from memory in the two versions?
- How does the interface of SoA change with respect to AoS?

## Bonus: matrix multiplication

`matrix.cc` multiplies two N × N matrices, stored row by row, in two ways: the naive loop, and
a version that transposes B first.

```shell
g++ -std=c++20 -Wall -O3 -g matrix.cc -o matrix
./matrix
```

- Which one is faster, and by how much? In the inner loop of each, which elements of B are read
  one after the other, and how far apart are they in memory?
- Look at the caches. Comment out one of the two methods in `main`, compile again, and run:

  ```shell
   perf stat -e cycles,instructions,L1-dcache-loads,L1-dcache-load-misses,cache-misses ./matrix
  ```

  Then do the same for the other method. How many L1 misses are there per multiplication?
  (The [perf page](perf.html) explains these counters.)
- Instead of transposing, swap the two inner loops of the naive version (`i, k, j` instead of
  `i, j, k`). Is the result still correct? Is it faster?
