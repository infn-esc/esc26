---
title: Finding memory problems with perf
layout: main
section: memory
---

# Finding memory problems with perf

The code is in
[`hands-on/memory/perf`]({{ site.github.repository_url }}/tree/main/hands-on/memory/perf).

Each program in this directory has **one** memory problem hidden among two or
three functions. For each one:

1. measure it with `perf stat` and decide what kind of problem it is;
2. find the function (and the line) with `perf record`, `perf report` and
   `perf annotate`;
3. fix it, and check with `perf stat` that the counter you blamed went down.

Each fix is a few lines. The point is to learn to read what perf tells you.

| Program | What it does |
|---|---|
| `events.cc` | processes 50 events, each with 5 million hits |
| `field.cc` | fills a 3000 × 3000 field, subtracts a pedestal, sums it |
| `particles.cc` | sorts 1 million particles by energy, sums the pt of the energetic ones |
| `channels.cc` | calibrates 2,000 events of 16,384 detector channels |

Build everything with `make`. The Makefile compiles with `-O2 -g
-fno-omit-frame-pointer`: `-g` lets perf show source lines, and the frame
pointer lets `perf record -g` show who called whom.

## perf in five minutes

**`perf stat`** runs a program and counts events:

```bash
perf stat ./field                     # a default set: time, cycles, instructions, branches...
perf stat -d ./field                  # adds cache counters
perf stat -e task-clock,cycles,instructions,cache-misses,L1-dcache-load-misses,dTLB-load-misses,page-faults ./field
perf list                             # all the events this machine knows
```

What to look at:

- **insn per cycle (IPC)** = instructions / cycles. Above 2: the core is busy
  doing work. Below 0.5: it is mostly waiting, usually for memory.
- **cache-misses**: loads that missed the last-level cache and went to RAM.
- **L1-dcache-load-misses**: loads that missed the first-level cache.
- **dTLB-load-misses**: loads whose address translation was not in the TLB;
  each one costs a page-table walk.
- **page-faults**: pages the kernel had to map, usually at the first touch of
  freshly allocated memory.
- **user** and **sys** time at the bottom: time in your code and time in the
  kernel. A large `sys` time in a program that does no I/O often means page
  faults.

**`perf record`** samples where the program spends its time, and **`perf
report`** shows the result:

```bash
perf record ./field                   # samples cycles by default; writes perf.data
perf report                           # interactive; or: perf report --stdio
perf record -e L1-dcache-load-misses ./field   # sample a specific event instead of cycles
perf record -g ./field                # also record call chains (who called the hot function)
perf record --call-graph dwarf ./events        # more reliable call chains, bigger perf.data
```

**`perf annotate`** shows the hot instructions of each function, with the
source lines next to them. The hottest function comes first (or press `a` on
a function in `perf report`):

```bash
perf annotate --stdio | less
```

The functions in these programs are marked `NOINLINE` so that the profile
shows them separately. In real code the compiler merges small functions into
their callers, and `perf annotate` is how you find the hot line inside a big
function.

Lines marked `[k]` in `perf report` are inside the kernel. If perf prints
`Couldn't record kernel reference relocation symbol`, kernel addresses are
hidden from users: you see hex numbers instead of kernel function names. That
is harmless here.

## Exercise 1: `events.cc`

```bash
make events
perf stat -e task-clock,cycles,instructions,page-faults ./events
```

- How many page faults are there? 
- Where do the faults come from? Record them with their call chains:

  ```bash
  perf record -e page-faults --call-graph dwarf ./events
  perf report --stdio --no-children
  ```

- Fix it, and check the number of page faults and the `sys` time again.

## Exercise 2: `field.cc`

```bash
make field
perf stat -e task-clock,cycles,instructions,cache-misses,L1-dcache-load-misses,dTLB-load-misses ./field
perf record ./field && perf report --stdio
```

- Fix it and compare `L1-dcache-load-misses` and `dTLB-load-misses` before
  and after.

## Exercise 3: `particles.cc`

```bash
make particles
perf stat -e task-clock,cycles,instructions,cache-misses,L1-dcache-load-misses ./particles
perf record ./particles && perf report --stdio
```

- The program prints the time of the setup and of the analysis separately.
  Which functions does `perf report` show, and what do they have in common?
- Fix it the cache misses and both times.

## Exercise 4: `channels.cc`

```bash
make channels
perf stat -e task-clock,cycles,instructions,L1-dcache-load-misses,cache-misses,dTLB-load-misses ./channels
perf record ./channels && perf report --stdio
```


