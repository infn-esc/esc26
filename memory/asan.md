---
title: AddressSanitizer
layout: main
section: memory
---

# AddressSanitizer and valgrind

The code is in
[`hands-on/memory/asan`]({{ site.github.repository_url }}/tree/main/hands-on/memory/asan).
Each of the five programs has one memory bug.

```shell
[username@esc26-01 ~]$ cd {{ site.github.repository_name }}/hands-on/memory/asan
```

| program | what it does |
|---|---|
| `overrun.cpp` | writes into a small `malloc`ed array |
| `array_too_small.cpp` | fills a vector; takes the number of elements as an argument |
| `double_delete.cpp` | creates and deletes an object |
| `leak.cpp` | creates two objects |
| `missed_delete.cpp` | creates an object, uses it, deletes it |

For each program:

1. Compile and run it normally. Does anything go wrong?

   ```shell
   [username@esc26-01 asan]$ g++ -g overrun.cpp -o overrun
   [username@esc26-01 asan]$ ./overrun
   ```

2. Compile it with AddressSanitizer and run it again. Read the report: what kind of error is
   it, on which line, and where was the memory allocated?

   ```shell
   [username@esc26-01 asan]$ g++ -g -fsanitize=address overrun.cpp -o overrun
   [username@esc26-01 asan]$ ./overrun
   ```

3. Run the normal build (without `-fsanitize`) with valgrind, and compare the two reports:

   ```shell
   [username@esc26-01 asan]$ g++ -g overrun.cpp -o overrun
   [username@esc26-01 asan]$ valgrind --leak-check=full ./overrun
   ```

4. Fix the bug, and check that both tools are happy.

Questions:

- `array_too_small`: run it with `./array_too_small 16`, then with 1000 and 100000, without
  AddressSanitizer. When does it crash? Is the program correct when it does not crash?
- `leak`: valgrind and AddressSanitizer do not report the same thing. What is the difference
  between "definitely lost" and "still reachable"?
- `missed_delete`: AddressSanitizer reports nothing. Why? What is the bug, and how do you
  write the code so that it cannot happen? (Hint: who calls `delete` if something throws?)
- Why do we compile with `-g`?
