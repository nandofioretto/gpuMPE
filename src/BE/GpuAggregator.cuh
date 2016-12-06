#ifndef CUDA_BE_GPU_AGGREGATOR_HPP
#define CUDA_BE_GPU_AGGREGATOR_HPP

#include <memory>
#include "Types.hpp"
#include "BE/GpuTable.cuh"

namespace Gpu {
  namespace Aggregator {
    // Out Table scope >= In Table scope
    void join(GpuTable::ptr& out, GpuTable::ptr& in);

    __global__ void cudaJoin(util_t* out, util_t* in,
			     size_t* mult, size_t* div, int* mod, 
			     int /*mult|div*/size,
			     size_t blockShift,
			     size_t threadGuard);
   };
};

#endif // CUDA_BE_GPU_AGGREGATOR_HPP
