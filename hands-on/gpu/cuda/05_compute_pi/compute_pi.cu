// ─────────────────────────────────────────────────────────────────────────────
// Exercise 04 – Computing π in Parallel  (STUDENT VERSION)
//
// Goal:
//   • Approximate π via the midpoint-rule numerical integration of
//       ∫₀¹ 4 / (1 + x²) dx = π
//   • Split [0, 1] into kNumSteps slices; each GPU thread evaluates the
//     integrand at the midpoint of exactly one slice and atomically adds
//     its contribution to a single global accumulator.
//   • Multiply the accumulated sum by the slice width to get the final
//     estimate, and compare it against the host's own computation.
//
// ─────────────────────────────────────────────────────────────────────────────

// C++ headers
#include <cassert>
#include <cmath>
#include <iostream>

// CUDA headers
#include <cuda_runtime.h>

// Local helper
#include "cuda_check.h"

// ---------------------------------------------------------------------------
// Configuration
// ---------------------------------------------------------------------------
constexpr int kDeviceId = 0;         // Change if you were assigned a different GPU
constexpr long kNumSteps = 1 << 20;  // Number of slices of [0, 1]
constexpr int kBlockSize = 256;      // Threads per block

// ---------------------------------------------------------------------------
// Kernel – each thread should add 4/(1+x²), evaluated at its slice's
// midpoint, into *d_sum
// ---------------------------------------------------------------------------
__global__ void computePiKernel(double* __restrict__ d_sum, long numSteps, double step) {
  // TODO: Implement the kernel
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main() {
  CUDA_CHECK(cudaSetDevice(kDeviceId));
  cudaStream_t stream;
  CUDA_CHECK(cudaStreamCreate(&stream));

  const double step = 1.0 / static_cast<double>(kNumSteps);

  // ───►►► Part 1 of 5 – allocate & zero-init the device accumulator ◄◄◄────
  // API: cudaMallocAsync(void** ptr, size_t size, cudaStream_t s)
  // API: cudaMemsetAsync(void* ptr, int value, size_t size, cudaStream_t s)
  double* d_sum = nullptr;
  // TODO: allocate d_sum (a single double)
  // TODO: zero-initialize *d_sum

  // ───►►► Part 2 of 5 – configure grid & block ◄◄◄─────────────────────────
  // TODO: compute numBlocks so that numBlocks * kBlockSize >= kNumSteps

  // ───►►► Part 3 of 5 – launch the kernel ◄◄◄──────────────────────────────
  // TODO: launch computePiKernel<<<numBlocks, kBlockSize, 0, stream>>>(...)
  // CUDA_CHECK(cudaGetLastError()); // check launch

  // ───►►► Part 4 of 5 – copy the accumulated sum back to the host ◄◄◄──────
  double sum = 0.0;
  // TODO: cudaMemcpyAsync d_sum -> sum (device to host)

  // Free device memory (async)
  // TODO: free d_sum

  // Wait for completion
  CUDA_CHECK(cudaStreamSynchronize(stream));

  const double piGpu = sum * step;

  // ───►►► Part 5 of 5 – verify against the host computation ◄◄◄───────────
  double hostSum = 0.0;
  for (long i = 0; i < kNumSteps; ++i) {
    const double x = (i + 0.5) * step;
    hostSum += 4.0 / (1.0 + x * x);
  }
  const double piHost = hostSum * step;

  std::cout << "GPU pi estimate:  " << piGpu << '\n';
  std::cout << "Host pi estimate: " << piHost << '\n';

  assert(std::fabs(piGpu - piHost) < 1e-9);
  assert(std::fabs(piGpu - M_PI) < 1e-6);

  CUDA_CHECK(cudaStreamDestroy(stream));
  std::cout << "Exercise 04 – compute pi: PASSED 🎉" << std::endl;
  return 0;
}
