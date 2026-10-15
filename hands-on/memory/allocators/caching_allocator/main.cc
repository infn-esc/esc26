#include "CachingAllocator.h"
#include "ParticleSoA.h"
#include "ParticleSoAVec.h"
#include <sys/resource.h>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <initializer_list>
#include <random>
#include <vector>

class Timer
{
  public:
    Timer() : start_time_(std::chrono::steady_clock::now()) {}

    void reset()
    {
        start_time_ = std::chrono::steady_clock::now();
    }

    double elapsed() const
    {
        auto                                      end_time     = std::chrono::steady_clock::now();
        std::chrono::duration<double, std::milli> elapsed_time = end_time - start_time_;
        return elapsed_time.count();
    }

  private:
    std::chrono::steady_clock::time_point start_time_;
};

// Number of page faults so far: every first touch of a page that the OS has
// not given to the program yet costs one
long pageFaults()
{
    rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_minflt;
}

// The work done on every "event": fill x and y, compute z = x + y, check it
bool compute(double* x, double* y, double* z, int N)
{
    for (int i = 0; i < N; ++i)
    {
        x[i] = static_cast<double>(i);
        y[i] = static_cast<double>(i * 2);
    }
    for (int i = 0; i < N; ++i)
    {
        z[i] = x[i] + y[i];
    }
    for (int i = 0; i < N; ++i)
    {
        if (z[i] != x[i] + y[i])
        {
            std::fprintf(stderr, "Result is incorrect!\n");
            return false;
        }
    }
    return true;
}

// Run `oneEvent(N)` once for every size in `sizes`, print time and page faults
template <typename F> void measure(const char* name, const std::vector<int>& sizes, F oneEvent)
{
    const long faults = pageFaults();
    Timer      timer;
    for (int N : sizes)
    {
        if (!oneEvent(N))
            std::exit(1);
    }
    std::printf("  %-36s %9.2f ms %9ld page faults\n", name, timer.elapsed(), pageFaults() - faults);
}

// A few allocations with the debug printout on, to see what the allocator does
void debugRun()
{
    std::printf("Debug run:\n");
    CachingAllocator allocator(true);
    for (int N : {8000, 6000, 30000000})
    {  // the last one is too big to cache
        void* p = allocator.allocate(sizeof(double) * N);
        allocator.deallocate(p);
    }
    std::printf("\n");
}

void benchmark(const char* title, int minN, int maxN, int NIter)
{
    // the same random sizes for every version
    std::mt19937                       gen(42);
    std::uniform_int_distribution<int> distribution(minN, maxN);
    std::vector<int>                   sizes(NIter);
    for (auto& N : sizes)
        N = distribution(gen);
    std::printf("%s: %d events, %d to %d doubles per array\n", title, NIter, minN, maxN);

    // +++++++ YOU CAN PRINT DEBUG INFO SETTING true TO THE ALLOCATOR CONSTRUCTOR
    // ++++++ BUT IF YOU WANT TO COMPARE TIMING RESULTS REMEMBER TO SET IT TO
    // false: "printing" slows down the program
    CachingAllocator allocator(false);
    measure("caching allocator",
            sizes,
            [&allocator](int N)
            {
                ParticleSoA particles;
                particles.x = static_cast<double*>(allocator.allocate(sizeof(double) * N));
                particles.y = static_cast<double*>(allocator.allocate(sizeof(double) * N));
                particles.z = static_cast<double*>(allocator.allocate(sizeof(double) * N));
                if (!particles.x || !particles.y || !particles.z)
                {
                    std::fprintf(stderr, "allocate() returned nullptr: write it first!\n");
                    return false;
                }
                const bool ok = compute(particles.x, particles.y, particles.z, N);
                allocator.deallocate(particles.z);
                allocator.deallocate(particles.y);
                allocator.deallocate(particles.x);
                return ok;
            });
    allocator.free();

    measure("malloc / free",
            sizes,
            [](int N)
            {
                ParticleSoA particles;
                particles.x   = static_cast<double*>(std::malloc(sizeof(double) * N));
                particles.y   = static_cast<double*>(std::malloc(sizeof(double) * N));
                particles.z   = static_cast<double*>(std::malloc(sizeof(double) * N));
                const bool ok = compute(particles.x, particles.y, particles.z, N);
                std::free(particles.z);
                std::free(particles.y);
                std::free(particles.x);
                return ok;
            });

    measure("std::vector<double>(N)",
            sizes,
            [](int N)
            {
                ParticleSoAVec particlesVec;
                particlesVec.x.resize(N);  // resize() also sets every element to 0
                particlesVec.y.resize(N);
                particlesVec.z.resize(N);
                return compute(particlesVec.x.data(), particlesVec.y.data(), particlesVec.z.data(), N);
            });

    measure("std::vector, push_back, no reserve",
            sizes,
            [](int N)
            {
                ParticleSoAVec particlesVec;
                for (int i = 0; i < N; ++i)
                {  // the vectors grow (and move) many times
                    particlesVec.x.push_back(0.);
                    particlesVec.y.push_back(0.);
                    particlesVec.z.push_back(0.);
                }
                return compute(particlesVec.x.data(), particlesVec.y.data(), particlesVec.z.data(), N);
            });
}

int main()
{
    debugRun();

    // warm-up: let the CPU reach full speed before measuring anything
    std::vector<double> x(1000000), y(1000000), z(1000000);
    for (int i = 0; i < 20; ++i)
        compute(x.data(), y.data(), z.data(), 1000000);

    benchmark("Small arrays", 5000, 10000, 1000);     // 40 to 80 KB
    benchmark("Large arrays", 5000000, 8000000, 30);  // 40 to 64 MB
    return 0;
}
