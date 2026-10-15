#include "timer.h"
#include <cstdio>
#include <vector>

// NOINLINE keeps each step a separate function, so that `perf report` can
// tell them apart (otherwise the compiler merges them all into main)
#define NOINLINE __attribute__((noinline))

constexpr int nChannels = 16384;
constexpr int nEvents   = 2000;

// What is used for every hit lives in its own small struct, packed next to
// the constants of the other channels: 16 bytes per channel instead of 4 KB,
// so all the calibrations fit in a few pages
struct Calibration
{
    double gain;
    double pedestal;
};

// let's keep this one separate since it is not used in computation!
struct Monitoring
{
    double history[500];  // the last 500 readings, for monitoring
};

struct Channels
{
    std::vector<Calibration> calibration;
    std::vector<Monitoring>  monitoring;
};

NOINLINE Channels makeChannels()
{
    Channels channels{std::vector<Calibration>(nChannels), std::vector<Monitoring>(nChannels)};
    for (int c = 0; c < nChannels; ++c)
    {
        channels.calibration[c].gain     = 1.0 + 0.001 * (c % 100);
        channels.calibration[c].pedestal = 0.5;
    }
    return channels;
}

// every event has one hit per channel
NOINLINE double calibrateEvent(const Channels& channels, int event)
{
    const auto& calibration = channels.calibration;
    double      energy      = 0;
    for (int c = 0; c < nChannels; ++c)
    {
        const double adc = (c + event) % 64;
        energy += adc * calibration[c].gain - calibration[c].pedestal;
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
    std::printf("sizeof(Calibration) %zu B   total %.6e   time %.1f ms\n", sizeof(Calibration), total, timer.elapsed());
}
