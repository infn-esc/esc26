#include "timer.h"
#include <cstdio>
#include <vector>

// NOINLINE keeps each step a separate function, so that `perf report` can
// tell them apart (otherwise the compiler merges them all into main)
#define NOINLINE __attribute__((noinline))

constexpr int nChannels = 16384;
constexpr int nEvents   = 2000;

struct Channel
{
    double gain;
    double pedestal;
    double history[500];  // the last 500 readings, for monitoring
};

NOINLINE std::vector<Channel> makeChannels()
{
    std::vector<Channel> channels(nChannels);
    for (int c = 0; c < nChannels; ++c)
    {
        channels[c].gain     = 1.0 + 0.001 * (c % 100);
        channels[c].pedestal = 0.5;
    }
    return channels;
}

// every event has one hit per channel
NOINLINE double calibrateEvent(const std::vector<Channel>& channels, int event)
{
    double energy = 0;
    for (int c = 0; c < nChannels; ++c)
    {
        const double adc = (c + event) % 64;
        energy += adc * channels[c].gain - channels[c].pedestal;
    }
    return energy;
}

int main()
{
    const auto channels = makeChannels();
    Timer      timer;
    double     total = 0;
    for (int event = 0; event < nEvents; ++event)
        total += calibrateEvent(channels, event);
    std::printf("sizeof(Channel) %zu B   total %.6e   time %.1f ms\n", sizeof(Channel), total, timer.elapsed());
}
