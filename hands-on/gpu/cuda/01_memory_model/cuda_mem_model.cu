// ─────────────────────────────────────────────────────────────────────────────
// Exercise 1 – CUDA Memory Model
//
// Fill in every line marked  ►►► TODO ◄◄◄.
// When you can build and run the program without assertions, you’re done.
//
// Build:   nvcc -std=c++17 memory_model.cu -o ex01
// ─────────────────────────────────────────────────────────────────────────────

// C++ headers
#include <cassert>
#include <iostream>
#include <vector>

// CUDA headers
#include <cuda_runtime.h>

// Local helper
#include "cuda_check.h"

// ---------------------------------------------------------------------------
// Configuration
// ---------------------------------------------------------------------------
constexpr int kDeviceId = 0;    // Change if you were assigned a different GPU
constexpr int kNumElements = 8; // Array length
using ValueT = float;

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main() {
  // 1. Select device and create a stream
  CUDA_CHECK(cudaSetDevice(kDeviceId));
  cudaStream_t stream;
  CUDA_CHECK(cudaStreamCreate(&stream));

  // 2. Host data – h_a = {0, 1, 2, …}
  std::vector<ValueT> h_a(kNumElements);
  for (int i = 0; i < kNumElements; ++i) {
    h_a[i] = static_cast<ValueT>(i);
  }

  // 3. Device buffers
  ValueT *d_a = nullptr;
  ValueT *d_b = nullptr;
  const std::size_t bytes = kNumElements * sizeof(ValueT);

  // ───►►► Part 1 of 5 – allocate device memory ◄◄◄───────────────────────────
  // Hint: cudaMallocAsync(void **ptr, size_t size, cudaStream_t stream)
  CUDA_CHECK(/* TODO: allocate d_a */);
  CUDA_CHECK(/* TODO: allocate d_b */);

  // ───►►► Part 2 of 5 – host → device copy ◄◄◄──────────────────────────────
  // Hint: cudaMemcpyAsync(dst, src, bytes, cudaMemcpyHostToDevice, stream)
  CUDA_CHECK(/* TODO: h_a → d_a */);

  // ───►►► Part 3 of 5 – device → device copy ◄◄◄────────────────────────────
  CUDA_CHECK(/* TODO: d_a → d_b */);

  // Clear host and copy back
  std::fill(h_a.begin(), h_a.end(), ValueT{0});

  // ───►►► Part 4 of 5 – device → host copy ◄◄◄──────────────────────────────
  CUDA_CHECK(/* TODO: d_b → h_a */);

  // ───►►► Part 5 of 5 – free device memory ◄◄◄──────────────────────────────
  CUDA_CHECK(/* TODO: free d_a */);
  CUDA_CHECK(/* TODO: free d_b */);

  // Wait for all asynchronous operations
  CUDA_CHECK(cudaStreamSynchronize(stream));

  // Verify result
  for (int i = 0; i < kNumElements; ++i) {
    assert(h_a[i] == static_cast<ValueT>(i));
  }

  CUDA_CHECK(cudaStreamDestroy(stream));
  std::cout << "Exercise 1 – memory model: PASSED 🎉" << std::endl;
  return 0;
}
