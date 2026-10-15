#include "Particle.h"
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <random>

// utility class for timing
class Timer {
public:
  Timer() : start_time_(std::chrono::steady_clock::now()) {}

  void reset() { start_time_ = std::chrono::steady_clock::now(); }

  double elapsed() const {
    auto end_time = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> elapsed_time =
        end_time - start_time_;
    return elapsed_time.count();
  }

private:
  std::chrono::steady_clock::time_point start_time_;
};

// function to initialize AoS
void initializeAoS(/*args*/) {
  // your code here
}

// function for computation on AoS
void computeOnAoS(std::vector<GoodParticle> &particles, const double t,
                  double x_max, int NIter) {
  for (int iter = 0; iter < NIter; ++iter) {
    // your code here
  }
}

// function to initialize SoA
void initializeSoA(/*args*/) {
  // your code here
}

// function for computation on SoA
void computeOnSoA(std::vector<double> &x_, std::vector<double> &px_,
                  const std::vector<float> &mass_,
                  std::vector<std::uint8_t> &hit_x_,
                  const double t, const double x_max, const int NPart,
                  const int NIter) {
  for (int iter = 0; iter < NIter; ++iter) {
    // your code here;
  }
}

int main() {
  std::mt19937 gen(2);

  std::vector<double> pxDist;
  std::vector<double> pyDist;
  std::vector<double> pzDist;
  std::vector<double> xDist;
  std::vector<double> zDist;
  std::vector<double> yDist;
  std::vector<float> massDist;

  constexpr int NIter = 100;
  constexpr double x_max = 200.;
  constexpr int Npart = 1000000;

  auto fillVec = [&]<typename T>(std::vector<T> &vec, const T min,
                                 const T max) {
    vec.reserve(Npart);
    std::uniform_real_distribution<double> dist(min, max);
    std::generate_n(std::back_inserter(vec), Npart, [&] { return dist(gen); });
  };

  fillVec(pxDist, 10., 100.);
  fillVec(pyDist, 10., 100.);
  fillVec(pzDist, 10., 100.);
  fillVec(xDist, 0., x_max); // every particle starts inside the box
  fillVec(yDist, -100., 100.);
  fillVec(zDist, -300., 300.);
  fillVec(massDist, 10.f, 100.f);

  auto const t = std::uniform_real_distribution<double>(-2., 2.)(gen);

  std::vector<GoodParticle> good_particles;
  initializeAoS(/*args*/);

  Timer timer; // starting the clock: time only the computation
  computeOnAoS(/*args*/);

  std::cout << "AoS Elapsed time: " << timer.elapsed() << " ms " << "\n";

  ParticleSoA soa_particles(Npart);
  initializeSoA(/*args*/);

  timer.reset(); // reset and restart the clock
  computeOnSoA(/*args*/);
  std::cout << "SoA Elapsed time: " << timer.elapsed() << " ms " << "\n";

  return 0;
}
