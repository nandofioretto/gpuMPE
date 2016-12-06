#include <iostream>
#include <utility>
#include <utils.hpp>

#include "Assert.cuh"
#include "Assert.hpp"
#include "Types.hpp"
#include "Preferences.hpp"
#include "BE/GpuProjector.cuh"
#include "BE/GpuTable.cuh"
#include "Utils/GpuAllocator.cuh"


std::pair<value_t, util_t>
Gpu::Projector::project(const GpuTable::ptr& in) {
  value_t bestVal = 0;
  util_t bestUtil = 0;

  std::vector<util_t> hUtilsIn(in->getSize());
  Gpu::Allocator::cpyToHost(&hUtilsIn[0], in->getUtils(), in->getSize()); 

  // Choose best Value
  for (int r=0; r<in->getSize(); r++) {
#ifndef MPE
    if (bestUtil RelOP hUtilsIn[r])
      bestVal = r;
#endif
    bestUtil = ProjOP(bestUtil, hUtilsIn[r]);
  }
  
  if (Preferences::verbose) {
    std::cout << "Best Val = " << bestVal << "\n";
  }
  return std::make_pair<>(bestVal, bestUtil);
}


GpuTable::ptr
Gpu::Projector::project(GpuTable::ptr& in, const Variable::ptr& var) {
  ASSERT(in->getScope().back()->getAgtID() == var->getAgtID(),
	 "Projection done over bad ordered variables");
  
  auto scope = in->getScope();
  std::vector<Variable::ptr> newScope(scope.begin(), scope.end()-1);
  size_t varDomSize = var->getDomSize();
  size_t newTabSize = in->getSize() / varDomSize;
  
  size_t nOutRows    = newTabSize;
  size_t nThreads    = varDomSize;
  size_t nBlocksLeft = nOutRows % nThreads == 0 ? (nOutRows / nThreads)
    : (nOutRows / nThreads) + 1;

  size_t blockShift = 0;
  while (nBlocksLeft > 0) {
    size_t nBlocks = nBlocksLeft > Gpu::Info::maxBlocks ? 
      Gpu::Info::maxBlocks : nBlocksLeft; 
    
    cudaProject<<<nBlocks, nThreads>>>(in->getUtils(),
				       varDomSize,
				       blockShift, 
				       newTabSize);
    cuCheck(cudaDeviceSynchronize());
    nBlocksLeft -= nBlocks;
    blockShift  += (nBlocks*nThreads);
  }

  // Gpu::Allocator::free(in->getUtils());
  in->update(newScope, newTabSize);

  return in;
}


// After projecting need an opreartion of compress to move all elements 
// projected on the top of the First Table
// Each Thread carries the projection for 1 Segment (D Util rows in the new 
// util table size) 
// 
__global__
void Gpu::Projector::cudaProject(util_t* out,
				int segmentSize,
				//int nSegments,
				size_t blockShift,
				size_t threadGuard) {

  size_t Tid = blockShift + (blockIdx.x * blockDim.x) + threadIdx.x;
  if (Tid >= threadGuard)
    return;
  size_t  outIdx  = (blockIdx.x * blockDim.x) + threadIdx.x; 
  size_t  inIdx   = outIdx * segmentSize;  

  // [todo] Copy in shared the amount of table to check?
  util_t bestItem = out[inIdx];
#pragma unroll
  for (int i=1; i<segmentSize; i++) {
    //maxItem = /*__nv_*/max(maxItem, out[inIdx+i]);
    bestItem = cudaProjOP(bestItem, out[inIdx+i]);
  }
  
  out[outIdx] = bestItem;
}
