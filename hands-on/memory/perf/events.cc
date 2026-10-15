#include "timer.h"
#include <cstdio>
#include <vector>

// NOINLINE keeps each step a separate function, so that `perf report` can
// tell them apart (otherwise the compiler merges them all into main)
#define NOINLINE __attribute__((noinline))

constexpr int         nEvents = 50;
constexpr std::size_t nHits   = 5'000'000;  // 40 MB of doubles per event

// random engine
NOINLINE void readout(std::vector<double>& hits, int event)
{
    unsigned x = 12345u + event;
    for (auto& h : hits)
    {
        x = x * 1664525u + 1013904223u;
        h = (x >> 8) * (1.0 / (1u << 24));
    }
}

NOINLINE void calibrate(std::vector<double>& hits)
{
    for (auto& h : hits)
        h = 0.98 * h + 0.01;
}

NOINLINE double sumEnergy(const std::vector<double>& hits)
{
    double s = 0;
    for (double h : hits)
        s += h;
    return s;
}

NOINLINE double processEvent(int event)
{
    std::vector<double> hits(nHits);  // a buffer for this event
    readout(hits, event);
    calibrate(hits);
    return sumEnergy(hits);
}

int main()
{
    Timer  timer;
    double total = 0;
    for (int event = 0; event < nEvents; ++event)
        total += processEvent(event);
    std::printf("total energy %.6e   time %.1f ms\n", total, timer.elapsed());
}
