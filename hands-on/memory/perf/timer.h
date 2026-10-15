#ifndef TIMER_H
#define TIMER_H
#include <chrono>

// utility class for timing
class Timer
{
  public:
    Timer() : start_time_(std::chrono::steady_clock::now()) {}
    void reset()
    {
        start_time_ = std::chrono::steady_clock::now();
    }
    double elapsed() const
    {  // milliseconds
        std::chrono::duration<double, std::milli> t = std::chrono::steady_clock::now() - start_time_;
        return t.count();
    }

  private:
    std::chrono::steady_clock::time_point start_time_;
};
#endif
