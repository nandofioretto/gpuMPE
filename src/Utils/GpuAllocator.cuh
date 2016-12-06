/*
 * gpu.cuh
 *
 *  Created on: Jan 13, 2016
 *      Author: ffiorett
 */

#ifndef CUDA_DBE_GPU_CUH
#define CUDA_DBE_GPU_CUH

#include <iostream>
#include <vector>
#include <algorithm>
#include <cuda.h>

#include "Types.hpp"
#include "Assert.cuh"
#include "Utils/GpuInfo.cuh"
#include "Utils/GpuTimer.cuh"
////////////////////////////////
// GPU-Device Utilities
////////////////////////////////
namespace Gpu {
  
  // Global Structures
  namespace Allocator {
    static std::vector<void*> d_dataPtr;
    static std::vector<size_t> d_dataSize;

    // Device Allocation Helper Functions
    template<class T>
    static T* alloc(const std::vector<T>& h_data) {
      T* d_data;
      size_t size = h_data.size()*sizeof(T);
	  Timer::start();
      cuCheck(cudaMalloc(&d_data, size));
      cuCheck(cudaMemcpy(d_data, &h_data[0], size, cudaMemcpyHostToDevice));
	  Timer::stop();
      Allocator::d_dataPtr.push_back(d_data);
      Allocator::d_dataSize.push_back(size);
      Info::incrGlobalMemory(size);
      
      // std::cout << "Storing GPU link: " << d_data << " size: " << h_data.size()
      // 		<< " Used Memory: " << Info::getUsedGlobalMemoryMB() << " MB\n";
      return d_data;
    }
    
    // Device Allocation Helper Functions
    template<class T>
    static T* alloc(size_t size, T* h_data) {
      T* d_data;
	  Timer::start();
      cuCheck(cudaMalloc(&d_data, size * sizeof(T)));
      cuCheck(cudaMemcpy(d_data, h_data, size * sizeof(T), cudaMemcpyHostToDevice));
	  Timer::stop();
      Allocator::d_dataPtr.push_back(d_data);
      Allocator::d_dataSize.push_back(size*sizeof(T));
      Info::incrGlobalMemory(size*sizeof(T));
      
      // std::cout << "Storing GPU link: " << d_data << " size: " << size
      // 		<< " Used Memory: " << Info::getUsedGlobalMemoryMB() << " MB\n";

      return d_data;
    }
    
    template<class T>
    static T* alloc(size_t size, T elem) {
      T* d_data;
	  Timer::start();
      cuCheck(cudaMalloc(&d_data, size * sizeof(T)));
	  std::vector<T>h_data(size, elem);
	  cuCheck(cudaMemcpy(d_data, &h_data[0], size *sizeof(T), cudaMemcpyHostToDevice));
	  // Todo: Can you make the above copy more efficient? Do not work for elements != 0
      // cuCheck(cudaMemset(d_data, elem, size * sizeof(T)));
	  Timer::stop();

      Allocator::d_dataPtr.push_back(d_data);
      Allocator::d_dataSize.push_back(size*sizeof(T));
      Info::incrGlobalMemory(size*sizeof(T));

      // std::cout << "Storing GPU link: " << d_data << " size : " << size
      // 		<< " Used Memory: " << Info::getUsedGlobalMemoryMB() << " MB\n";

      return d_data;
    }
    
    
    template<class T>
    static void cpyToDevice(T* d_data, T* h_data, size_t size) {
  	  Timer::start();
      cuCheck(cudaMemcpy(d_data, h_data, size * sizeof(T), cudaMemcpyHostToDevice));
	  Timer::stop();
    }

    template<class T>
    static void cpyToHost(T* h_data, T* d_data, size_t size) {
  	  Timer::start();
      cuCheck(cudaMemcpy(h_data, d_data, size*sizeof(T), cudaMemcpyDeviceToHost));
	  Timer::stop();
    }
    
    
    static void free(void* d_ptr) {
      auto itPtr = std::find(d_dataPtr.begin(), d_dataPtr.end(), d_ptr);
      if (itPtr != d_dataPtr.end()) {
	if(d_ptr != nullptr) {
	  cuCheck(cudaFree(d_ptr));
	}
	auto itSize = d_dataSize.begin() + std::distance(d_dataPtr.begin(), itPtr);
	Info::decrGlobalMemory(*itSize);
	d_dataPtr.erase(itPtr);
	d_dataSize.erase(itSize);

	// std::cout << "Removing GPU link: " << d_ptr 
	// 	  << " Used Memory: " << Info::getUsedGlobalMemoryMB() << " MB\n";
      }
    }
    
    static void freeAll() {
      for (auto d_ptr : d_dataPtr) {
	free(d_ptr);
      }
      d_dataPtr.clear();
    }
    
  };
  
};


#endif /* GPU_CUH_ */
