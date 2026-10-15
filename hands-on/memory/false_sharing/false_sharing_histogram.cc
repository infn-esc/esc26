#include <chrono>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>
#include <vector>

constexpr size_t numThreads = 4;
constexpr size_t nBins      = 4;          // 4 unsigned int = 16 bytes per histogram
constexpr size_t nValues    = 1UL << 26;  // 64 M values, split between the threads

template <typename TResolution> class Timer
{
  public:
    Timer() : start_time_(std::chrono::high_resolution_clock::now()) {}

    void reset()
    {
        start_time_ = std::chrono::high_resolution_clock::now();
    }

    double elapsed() const
    {
        auto                                       end_time     = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, TResolution> elapsed_time = end_time - start_time_;
        return elapsed_time.count();
    }

  private:
    std::chrono::high_resolution_clock::time_point start_time_;
};

// Mutex for output
std::mutex printMutex;

// Every thread fills its own histogram: no two threads ever touch the same
// histogram, so we need no mutex and no atomics.
struct Histogram
{
    unsigned int bins[nBins] = {};
};

// fix this one!
struct GoodHistogram
{
    unsigned int bins[nBins] = {};
};

// alignas(64) makes Data start at a cache-line boundary, so the memory layout
// is the same in every run. It does not move the histograms apart.
template <typename THist> struct alignas(64) Data
{
    THist hist[numThreads];

    // Thread `index` fills hist[index] from its own slice of the values
    void fill(size_t index, const std::vector<std::uint8_t>& values)
    {
        const size_t chunk = values.size() / numThreads;
        for (size_t i = index * chunk; i < (index + 1) * chunk; ++i)
        {
            hist[index].bins[values[i]]++;
        }
#ifdef DEBUG
        // compile with -DDEBUG
        {
            std::lock_guard<std::mutex> lock(printMutex);
            std::cout << "Thread " << index << " filling hist[" << index << "] at address: " << &(hist[index])
                      << std::endl;
        }
#endif
    }
};

// Fill one histogram per thread and return the time it took in milliseconds
template <typename THist> double fillHistograms(const std::vector<std::uint8_t>& values)
{
    Data<THist>              data;
    std::vector<std::thread> threads;

    Timer<std::milli> timer;
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&data, &values, i]() { data.fill(i, values); });
    }
    for (auto& thread : threads)
    {
        thread.join();
    }
    double time = timer.elapsed();

    // Check: every value must have been counted exactly once
    size_t entries = 0;
    for (auto& h : data.hist)
    {
        for (auto b : h.bins)
        {
            entries += b;
        }
    }
    if (entries != values.size())
    {
        std::cout << "Wrong number of entries: " << entries << std::endl;
    }
    return time;
}

int main()
{
    // Our "data": random values between 0 and nBins - 1
    std::vector<std::uint8_t>          values(nValues);
    std::mt19937                       gen(42);
    std::uniform_int_distribution<int> dist(0, nBins - 1);
    for (auto& v : values)
    {
        v = dist(gen);
    }

    std::cout << "sizeof(Histogram) = " << sizeof(Histogram) << ", sizeof(GoodHistogram) = " << sizeof(GoodHistogram)
              << std::endl;

    // Repeat a few times: the first round is often slower, while the CPU
    // frequency ramps up
    for (int round = 0; round < 3; ++round)
    {
        double t      = fillHistograms<Histogram>(values);
        double good_t = fillHistograms<GoodHistogram>(values);
        std::cout << "Histogram: " << t << " ms   GoodHistogram: " << good_t << " ms" << std::endl;
    }

    return 0;
}
