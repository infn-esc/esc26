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

    void* allocate(std::size_t size)
    {
        if (debug_)
            std::printf("Asking for %zu Bytes\n", size);

        // use method to find the bin index for the requested allocation
        const std::size_t binIndex = findBin(size);
        if (binIndex > MAX_BIN)
        {
            // too big to be cached: allocate exactly what is asked (rounded up to a
            // multiple of ALIGNMENT, as std::aligned_alloc requires)
            large_blocks.emplace_back(roundUp(size));
            if (debug_)
                std::printf("Too big for the cache, allocating %p of size %zu Bytes\n",
                            static_cast<void*>(large_blocks.back().ptr),
                            large_blocks.back().size);
            return large_blocks.back().ptr;
        }

        auto& cached = cached_blocks[binIndex];
        if (!cached.empty())
        {
            // reuse the block that was returned last to this bin
            // (MemoryBlock is just a pointer and a size: copying it is cheap)
            const MemoryBlock block = cached.back();
            cached.pop_back();
            live_blocks[binIndex].push_back(block);
            if (debug_)
                std::printf("Reusing block %p of size %zu Bytes, bin %zu\n",
                            static_cast<void*>(block.ptr),
                            block.size,
                            binIndex);
            return block.ptr;
        }

        // nothing cached in this bin: create a new block
        live_blocks[binIndex].emplace_back(bin_sizes[binIndex]);
        if (debug_)
            std::printf("No cached block found, creating new one %p of size %zu "
                        "Bytes, bin %zu\n",
                        static_cast<void*>(live_blocks[binIndex].back().ptr),
                        live_blocks[binIndex].back().size,
                        binIndex);
        return live_blocks[binIndex].back().ptr;
    }

    void deallocate(void* ptr)
    {
        // Find the block among the live ones. A linear search is fine for a few
        // blocks; a real allocator stores the bin next to the block (in a small
        // header in front of it) or uses a hash map from pointer to block.
        auto samePtr = [ptr](const MemoryBlock& block) { return block.ptr == ptr; };
        for (std::size_t binIndex = MIN_BIN; binIndex <= MAX_BIN; ++binIndex)
        {
            auto& live = live_blocks[binIndex];
            auto  it   = std::find_if(live.begin(), live.end(), samePtr);
            if (it != live.end())
            {
                if (debug_)
                    std::printf("Caching block %p of size %zu Bytes in bin %zu\n", ptr, it->size, binIndex);
                // do not free the memory: keep the block for the next allocate()
                cached_blocks[binIndex].push_back(*it);
                live.erase(it);
                return;
            }
        }
        // blocks that were too big to be cached are freed immediately
        auto it = std::find_if(large_blocks.begin(), large_blocks.end(), samePtr);
        if (it != large_blocks.end())
        {
            if (debug_)
                std::printf("Freeing large block %p\n", ptr);
            std::free(it->ptr);
            large_blocks.erase(it);
            return;
        }
        if (debug_)
            std::printf("Unable to deallocate block %p\n", ptr);
    }

    // Release all the memory, live and cached. The allocator can still be used
    // afterwards.
    void free()
    {
        for (auto* bins : {&live_blocks, &cached_blocks})
        {
            for (auto& bin : *bins)
            {
                for (auto& block : bin)
                {
                    if (debug_)
                        std::printf("Block %p getting released\n", static_cast<void*>(block.ptr));
                    std::free(block.ptr);
                }
                bin.clear();  // keep the bins themselves, only empty them
            }
        }
        for (auto& block : large_blocks)
            std::free(block.ptr);
        large_blocks.clear();
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
    std::vector<MemoryBlock>              large_blocks;   // in use, too big to be cached
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

    // The smallest bin whose blocks are big enough for `size` bytes, or
    // MAX_BIN + 1 if the request is too big to be cached.
    std::size_t findBin(std::size_t size) const
    {
        std::size_t binIndex = MIN_BIN;
        while (binIndex <= MAX_BIN && bin_sizes[binIndex] < size)
            ++binIndex;
        return binIndex;
    }

    // std::aligned_alloc needs a size that is a multiple of the alignment
    static std::size_t roundUp(std::size_t size)
    {
        return (size + ALIGNMENT - 1) / ALIGNMENT * ALIGNMENT;
    }
};
#endif
