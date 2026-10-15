---
title: False sharing
layout: main
section: memory
---

# False sharing

The code is in
[`hands-on/memory/false_sharing`]({{ site.github.repository_url }}/tree/main/hands-on/memory/false_sharing).

## Part A: counters

Two threads increment two different counters, `counters[0]` and `counters[1]`. Modify
`false_sharing.cc`: change `GoodCounter` so that there is no false sharing.

```shell
g++ -O0 -pthread false_sharing.cc -o false_sharing
./false_sharing
```

- Compare the time of `Counter` and `GoodCounter`. Compile with `-DDEBUG` to print the address
  of each counter: how far apart are they?
- Now compile with `-O1`, `-O2` and `-O3`. What happens to the timing?

## Part B: histograms

`false_sharing_histogram.cc` uses a more realistic pattern: every thread fills its own small
histogram from its own part of the data, and the histograms sit next to each other in memory.

Modify `GoodHistogram` so that there is no false sharing.

```shell
g++ -O2 -pthread false_sharing_histogram.cc -o false_sharing_histogram
./false_sharing_histogram
```

## Part C: the best fix

Padding wastes memory, and the shared histograms are still written at every iteration. Add a
version of `fill` that counts in a local histogram (a local array inside the function) and
writes `hist[index]` only once, at the end.

