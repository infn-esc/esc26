#include "timer.h"
#include <cmath>
#include <cstdio>
#include <algorithm>
#include <vector>

// NOINLINE keeps each step a separate function, so that `perf report` can
// tell them apart (otherwise the compiler merges them all into main)
#define NOINLINE __attribute__((noinline))

struct Particle
{
    double energy;
    double px, py, pz;
};

constexpr int nParticles = 1'000'000;

// create the particles (random energies) and sort them by energy
NOINLINE std::vector<Particle> makeParticles()
{
    // std::sort moves the particles themselves: in a vector, sorted order is
    // also memory order. std::list::sort only relinks the nodes, which stay where
    // they were allocated, so walking the sorted list jumps around in memory.
    std::vector<Particle> particles;
    particles.reserve(nParticles);
    unsigned x   = 42;
    auto     rnd = [&x]
    {
        x = x * 1664525u + 1013904223u;
        return (x >> 8) * (1.0 / (1u << 24));
    };
    for (int i = 0; i < nParticles; ++i)
        particles.push_back({100 * rnd(), rnd(), rnd(), rnd()});
    std::sort(
        particles.begin(), particles.end(), [](const Particle& a, const Particle& b) { return a.energy < b.energy; });
    return particles;
}

NOINLINE double sumPt(const std::vector<Particle>& particles, double threshold)
{
    double s = 0;
    for (const auto& p : particles)
        if (p.energy > threshold)
            s += std::sqrt(p.px * p.px + p.py * p.py);
    return s;
}

int main()
{
    Timer        timer;
    const auto   particles = makeParticles();
    const double tSetup    = timer.elapsed();

    timer.reset();
    double s = 0;
    for (int pass = 0; pass < 4; ++pass)
        s += sumPt(particles, 50.0);
    std::printf("sum pt %.6e   setup %.1f ms   analysis %.1f ms\n", s, tSetup, timer.elapsed());
}
