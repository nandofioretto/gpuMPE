#include <iostream>
#include <utils.hpp>
#include <string_utils.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>

#include "Assert.cuh"
#include "Types.hpp"
#include "Utils/GpuAllocator.cuh"
#include "Utils/GpuInfo.cuh"
#include "BE/GpuTable.cuh"
#include "BE/GpuAggregator.cuh"

using namespace misc_utils;

void Gpu::Aggregator::join(GpuTable::ptr& out, GpuTable::ptr& in) {
  const int inScopeSize = in->getScope().size();
  const int outScopeSize = out->getScope().size();
  
  // Vector of shift factors
  std::vector<int> InOutIdx(inScopeSize, 0);
  // Populate vector of shift factors - with the associated indexes 
  // in the Out Table
  for (int i_in = 0; i_in < inScopeSize; i_in++) {
    auto in_var = in->getScope()[i_in];
    for (int i_out = 0; i_out < outScopeSize; i_out++) {
      auto out_var = out->getScope()[i_out];
      if (in_var->getAgtID() == out_var->getAgtID()) {
        InOutIdx[i_in] = i_out;
        break;
      }
    }
  }
  
  // 1. Compute vectors  mult, div, and mod
  std::vector<size_t> mult(inScopeSize-1, 1);
  std::vector<size_t> div(inScopeSize-1, 1);
  std::vector<int> mod(inScopeSize);
  
  for (int k=0; k<inScopeSize-1; k++) {
    // Mult
    for (int j=k+1; j<inScopeSize; j++)
      mult[k] *= in->getScope()[j]->getDomSize();
    
    // Div
    for (int j=InOutIdx[k]+1; j<outScopeSize; j++)
      div[k] *= out->getScope()[j]->getDomSize();

    // Mod
    mod[k] = in->getScope()[k]->getDomSize();
  }
  mod[inScopeSize-1] = in->getScope().back()->getDomSize();
  
  // 2. Copy data to device (this can also be done in preprocessing)
  auto d_mult = Gpu::Allocator::alloc(mult);
  auto d_div  = Gpu::Allocator::alloc(div);
  auto d_mod  = Gpu::Allocator::alloc(mod);

  // 3. Call kernel 
  size_t nOutRows    = out->getSize();
  size_t nThreads    = 256;
  size_t nBlocksLeft = nOutRows % nThreads == 0 ? (nOutRows / nThreads)
    : (nOutRows / nThreads) + 1;
 

  size_t blockShift = 0;
  while (nBlocksLeft > 0) {
    size_t nBlocks = nBlocksLeft > Gpu::Info::maxBlocks ? 
      Gpu::Info::maxBlocks : nBlocksLeft; 

    // std::cout << "Calling Aggregator GpuKernel with  (" 
    //           << nBlocks << ", " << nThreads << " Left:" << nBlocksLeft 
    //           << " maxBlocks "<< Gpu::Info::maxBlocks << ")\n"; 
   
    cudaJoin<<<nBlocks, nThreads>>>(out->getUtils(), in->getUtils(),
                                    d_mult, d_div, d_mod, inScopeSize-1,
                                    blockShift, nOutRows);
    cuCheck(cudaDeviceSynchronize());
    nBlocksLeft -= nBlocks;
    blockShift  += (nBlocks*nThreads);
  }
  
  // Free Temp Gpu Memory
  // [todo] mult, div and mod, as one single vector;
  Gpu::Allocator::free(d_mult);
  Gpu::Allocator::free(d_div);
  Gpu::Allocator::free(d_mod);
  
  // [todo] Delegate destroy and copy back (if necessary) to some other function.
}


// mod size = mult|div size + 1
__global__ 
void Gpu::Aggregator::cudaJoin(util_t* out, util_t* in,
			       size_t* mult, size_t* div, int* mod, 
			       int /*mult|div*/size,
			       size_t blockShift,
			       size_t threadGuard)
{
  size_t Tid = blockShift + (blockIdx.x * blockDim.x) + threadIdx.x;
  if (Tid >= threadGuard)
    return;
  // [todo] Copy into shared mult, div, mod.
  // [todo] Put mult div and mod in one single vector.

  // Find input Table index associated to this outputIdx
  size_t inIdx = 0;
#pragma unroll
  for (int i=0; i<size; i++) {
    inIdx += mult[i] * ((int)(Tid/div[i]) % mod[i]);
  }
  inIdx += Tid % mod[size];

  out[Tid] = out[Tid] cudaJoinOP in[inIdx];
}

