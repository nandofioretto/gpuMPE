/*
 * cpuAllocator.hpp
 *
 *  Created on: Jan 13, 2016
 *      Author: ffiorett
 */

#ifndef CUDA_DBE_CPUALLOCATOR_HPP
#define CUDA_DBE_CPUALLOCATOR_HPP

#include <iostream>
#include <random>
#include <vector>

namespace Cpu {
  namespace Allocator {
    static std::vector<void*> h_dataPtr;
    
    // Device Allocation Helper Functions
    template<class T>
      static T* alloc(size_t size, T value) {
      T* h_data = (T*)malloc(size * sizeof(T));
      for (size_t i=0; i< size; i++)
	h_data[i] = value;
      Allocator::h_dataPtr.push_back(h_data);
      return h_data;
    }
    
    // Device Allocation Helper Functions
    template<class T>
      static T* allocRnd(size_t size, T lb, T ub) {
      T* h_data = (T*)malloc(size * sizeof(T));
      std::default_random_engine generator;
      std::uniform_real_distribution<float> distribution(lb, ub);
      
      for (int i=0; i<size; ++i) {
	h_data[i] = (T)distribution(generator);
      }
      Allocator::h_dataPtr.push_back(h_data);
      return h_data;
    }
    
    // Device Allocation Helper Functions
    template<class T>
      static T* allocOrd(int plb, int pub, int llb, int lub, T step) {
      //			assert(optH == 1);
      //			// todo: later we need to sobstituite this function with the permutation generator
      
      int sizeG = ((pub - plb) + 1) / step;
      int sizeL = ((lub - llb) + 1) / step;
      size_t nRows = sizeG * sizeL;
      
      T* h_data = (T*)malloc((2*nRows) * sizeof(T));
      
      int i=0;
      for (T p = plb; p <= pub; p += step) {
	for (T l = llb; l <= lub; l += step) {
	  h_data[i++] = p;
	  h_data[i++] = l;
	}
      }
      
      Allocator::h_dataPtr.push_back(h_data);
      return h_data;
    }
    
    
    static void freeAll() {
      for (auto h_ptr : h_dataPtr)
	free(h_ptr);
      h_dataPtr.clear();
    }
  };
};



#endif /* CPUALLOCATOR_HPP_ */
