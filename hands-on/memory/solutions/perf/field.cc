#include "timer.h"
#include <cstdio>
#include <vector>

// NOINLINE keeps each step a separate function, so that `perf report` can
// tell them apart (otherwise the compiler merges them all into main)
#define NOINLINE __attribute__((noinline))

constexpr int nRows = 3000;
constexpr int nCols = 3000;  // 3000 x 3000 doubles = 72 MB

NOINLINE void fill(std::vector<double>& f)
{
    for (int row = 0; row < nRows; ++row)
        for (int col = 0; col < nCols; ++col)
            f[row * nCols + col] = 1.0 + (row + col) % 7;
}

// subtract a pedestal from every cell
// The inner loop must run over the last index (col), so that consecutive
// iterations touch consecutive addresses: one cache line serves 8 doubles.
NOINLINE void subtractPedestal(std::vector<double>& f, double pedestal)
{
    for (int row = 0; row < nRows; ++row)
        for (int col = 0; col < nCols; ++col)
            f[row * nCols + col] -= pedestal;
}

NOINLINE double total(const std::vector<double>& f)
{
    double s = 0;
    for (int row = 0; row < nRows; ++row)
        for (int col = 0; col < nCols; ++col)
            s += f[row * nCols + col];
    return s;
}

int main()
{
    std::vector<double> f(static_cast<std::size_t>(nRows) * nCols);
    Timer               timer;
    double              s = 0;
    for (int pass = 0; pass < 10; ++pass)
    {
        fill(f);
        subtractPedestal(f, 0.5);
        s += total(f);
    }
    std::printf("sum %.6e   time %.1f ms\n", s, timer.elapsed());
}
