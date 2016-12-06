#ifndef CUDA_BE_GPUINFO_CUH
#define CUDA_BE_GPUINFO_CUH

/* Common dependencies */
#include <stdlib.h> // for atoi
#include <sys/time.h>

/*
 * Info for Current CUDA card.
 * TODO: This should go in cuda utils
 */
namespace Gpu {

  struct Info {
  public:
    static size_t globalMemory; // in bytes
    static size_t sharedMemory; // in bytes
    static size_t maxThreads; // PerBlock;
    static size_t maxBlocks;  // dim 0;

    static size_t usedGlobalMemory;
    static size_t usedSharedMemory;
    static size_t usedTextureMemory;
    static size_t usedConstantMemory;
    
    static bool checkMemory() {
      return (usedGlobalMemory < globalMemory);
    }
    
    static void incrGlobalMemory(size_t mem) {
      usedGlobalMemory += mem;
    }

    static void decrGlobalMemory(size_t mem) {
      usedGlobalMemory -= mem;
    }

    static void initialize();

    static float getAvailableGlobalMemoryMB() {
      return (globalMemory - usedGlobalMemory) / 1000000.00;
    }

    static float getUsedGlobalMemoryMB() {
      return (usedGlobalMemory) / 1000000.00;
    }

    static float getAvailableGlobalMemoryBytes() {
      return (globalMemory - usedGlobalMemory);
    }

    static float getUsedGlobalMemoryBytes() {
      return usedGlobalMemory;
    }

  };

};

#endif
