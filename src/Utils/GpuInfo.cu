// CUDA and CUBLAS functions
// CUDA runtime
#include <cuda_runtime.h>

// Utilities and system includes
#include <assert.h>
//#include <helper_string.h>  // helper for shared functions common to CUDA Samples
#include <helper_functions.h>
#include <helper_cuda.h>

#include <vector>
#include <string>
#include <iostream>

#include "Assert.cuh"
#include "Utils/GpuInfo.cuh"
#include "Preferences.hpp"

size_t Gpu::Info::globalMemory; // in bytes
size_t Gpu::Info::sharedMemory; // in bytes
size_t Gpu::Info::maxThreads; // PerBlock;
size_t Gpu::Info::maxBlocks;  // dim 0;

size_t Gpu::Info::usedGlobalMemory;
size_t Gpu::Info::usedSharedMemory;
size_t Gpu::Info::usedTextureMemory;
size_t Gpu::Info::usedConstantMemory;


void 
Gpu::Info::initialize() {
  // get number of SMs on this GPU
  cudaDeviceProp deviceProp;
  cuCheck(cudaGetDeviceProperties(&deviceProp, Preferences::gpuDevID));
  
  if (Preferences::maxDevMemory == 0 || 
      Preferences::maxDevMemory > deviceProp.totalGlobalMem) {
    globalMemory = deviceProp.totalGlobalMem;
  } else {
    globalMemory = Preferences::maxDevMemory;
  }
  
  sharedMemory = deviceProp.sharedMemPerBlock;
  maxBlocks    = deviceProp.maxGridSize[0];
  maxThreads   = deviceProp.maxThreadsPerBlock;
  
  if (!Preferences::silent) {
    printf("GPU Device %d: \"%s\" with compute capability %d.%d\t"
	   "Global Memory: %.2f gb shared memory: %zu bytes\n", 
	     Preferences::gpuDevID,
	     deviceProp.name, deviceProp.major, deviceProp.minor,
	     globalMemory / 1000000000.00,
	     sharedMemory);
      }
  
  // use a larger block size for Fermi and above
  //int block_size = (deviceProp.major < 2) ? 16 : 32;
}