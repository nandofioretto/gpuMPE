#ifndef CUDA_BE_GPU_PROJECTOR_CUH
#define CUDA_BE_GPU_PROJECTOR_CUH

#include <memory>
#include <utility>

#include "GpuTable.cuh"
#include "Variable.hpp"

using namespace misc_utils;

// Done as if we were projecting out the last variable of the table 
namespace Gpu {
  namespace Projector {
    // Out Table scope >= In Table scope
    GpuTable::ptr project(GpuTable::ptr& out, const Variable::ptr& var);
    
    __global__
    void cudaProject(util_t* out,
		     int segmentSize,
		     //int nSegments,
		     size_t blockShift,
		     size_t threadGuard);

    std::pair<value_t, util_t> project(const GpuTable::ptr& in);
  };
};

#endif // CUDA_BE_CPU_PROJECTOR_HPP
