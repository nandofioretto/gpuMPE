#ifndef CUDA_BE_CPUINFO_CUH
#define CUDA_BE_CPUINFO_CUH

/* Common dependencies */
#include <stdlib.h> // for atoi
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <string>

#include "Assert.hpp"
#include "Preferences.hpp"

/*
 * Info for Current CUDA card.
 * TODO: This should go in cuda utils
 */
namespace Cpu {

  struct Info {
  public:
    static size_t globalMemory; // in bytes(take it from Preferences)
    static size_t usedGlobalMemory;
        
    static bool checkMemory() {
      return (usedGlobalMemory < globalMemory);
    }
    
    static void incrGlobalMemory(size_t mem) {
      usedGlobalMemory += mem;
      // if (Preferences::verbose)
      //   std::cout << "Increasing Memory: "
      //             << mem / 1000000000.00 << "GB - Device Memory Available: "
      //             << (getAvailableGlobalMemoryMB()/1000.00) 
      //             << "\n";

      if (Preferences::silent) { 
        if(!checkMemory()) { 
          std::cout << "NA\tNA\tNA\n";
          exit(1);
        }
      } else {
        ASSERT(checkMemory(), "Device Memory Used: "
               + std::to_string(getUsedGlobalMemoryMB()/1000.00)
               + "GB Exceeds Capacity: " 
               + std::to_string(globalMemory/1000000000.00) + "GB");
      }
    }

    static void decrGlobalMemory(size_t mem) {
      // if (Preferences::verbose)
      //   std::cout << "Decreasing Memory: " 
      //             << mem / 1000000000.00 << "GB - Device Memory Available: "
      //             << (getAvailableGlobalMemoryMB()/1000.00) 
      //             << "\n";
      usedGlobalMemory -= mem;
    }

    static void initialize() {
      globalMemory = 0;
      usedGlobalMemory = 0;
      long pages = sysconf(_SC_PHYS_PAGES);
      long page_size = sysconf(_SC_PAGE_SIZE);
      size_t mem = pages * page_size;

      if (Preferences::maxHostMemory == 0 ||
          Preferences::maxHostMemory > mem) {
        globalMemory = mem;
      } else {
        globalMemory = Preferences::maxHostMemory;
      }
      if(Preferences::verbose)
        std::cout << "Max Memory = " << globalMemory / 1000000000.00
                  << " GB\n";
    }

    static float getAvailableGlobalMemoryMB() {
      return (globalMemory - usedGlobalMemory) / 1000000.00;
    }

    static float getUsedGlobalMemoryMB() {
      return (usedGlobalMemory) / 1000000.00;
    }

    static size_t getUsedGlobalMemoryBytes() {
      return usedGlobalMemory;
    }

    static size_t getAvailableGlobalMemoryBytes() {
      return (globalMemory - usedGlobalMemory);
    }

  };

};

#endif
