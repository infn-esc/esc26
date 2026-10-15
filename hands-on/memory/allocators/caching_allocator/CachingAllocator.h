#ifndef CachingAllocator_h
#define CachingAllocator_h
#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <new>
#include <vector>

// Bin i holds blocks of GROWTH^i bytes. Only the bins from MIN_BIN to MAX_BIN
// are used (512 B ... 128 MiB): smaller requests get a block of the MIN_BIN
// size, larger requests are not cached at all (allocated and freed directly).
constexpr std::size_t GROWTH    = 8;
constexpr std::size_t MIN_BIN   = 3;
constexpr std::size_t MAX_BIN   = 9;
constexpr std::size_t ALIGNMENT = 64;  // every block starts on a cache line

class CachingAllocator
{
  public:
    CachingAllocator(bool debug = false) : debug_(debug)
    {
        initializeBins();
        precalculateBinSizes();
    }

    ~CachingAllocator()
    {
        free();
    }

    // Copying an allocator would make two objects free the same blocks
    CachingAllocator(const CachingAllocator&)            = delete;
    CachingAllocator& operator=(const CachingAllocator&) = delete;

    // Return a block of at least `size` bytes: a cached one if the right bin has
    // one, otherwise a new one. Requests bigger than the largest bin are not
    // cached: allocate them directly.
    void* allocate(std::size_t size)
    {
        if (debug_)
            std::printf("Asking for %zu Bytes\n", size);
        // your code here
        return nullptr;
    }

    // Do not free the memory: move the block from the live blocks to the
    // cached blocks of its bin, so that the next allocate() can reuse it
    void deallocate(void* ptr)
    {
        // your code here
    }

    // Release all the memory, live and cached
    void free()
    {
        // your code here
    }

  private:
    struct MemoryBlock
    {                            // this class is used to represent blocks we are going
                                 // to allocate
        MemoryBlock() = delete;  // I remove the default constructor because I want
                                 // to create a block only when I know the size
        MemoryBlock(std::size_t allocSize)
            : ptr(static_cast<std::byte*>(std::aligned_alloc(ALIGNMENT, allocSize))), size(allocSize)
        {
            if (ptr == nullptr)
                throw std::bad_alloc();
        }

        std::byte*  ptr;
        std::size_t size;
    };
    bool debug_;

    // one entry per bin, from 0 to MAX_BIN (the bins below MIN_BIN stay empty)
    std::vector<std::vector<MemoryBlock>> live_blocks;    // currently in use
    std::vector<std::vector<MemoryBlock>> cached_blocks;  // currently cached
    std::vector<std::size_t>              bin_sizes;

    void initializeBins()
    {
        live_blocks.resize(MAX_BIN + 1);
        cached_blocks.resize(MAX_BIN + 1);
    }

    void precalculateBinSizes()
    {
        std::size_t size = 1;
        for (std::size_t i = 0; i <= MAX_BIN; ++i)
        {
            bin_sizes.push_back(size);  // GROWTH^i
            size *= GROWTH;
        }
    }

    // you will need a function that finds the bin for a given size

    // std::aligned_alloc needs a size that is a multiple of the alignment
    static std::size_t roundUp(std::size_t size)
    {
        return (size + ALIGNMENT - 1) / ALIGNMENT * ALIGNMENT;
    }
};
#endif
