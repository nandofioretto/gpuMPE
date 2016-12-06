#ifndef CUDA_BE_CPU_AGGREGATOR_HPP
#define CUDA_BE_CPU_AGGREGATOR_HPP

#include <memory>
#include "CpuTable.hpp"

class CpuAggregator {
public:

  // Out Table scope >= In Table scope
  static void join(CpuTable::ptr& out, const CpuTable::ptr& in);

};

#endif // CUDA_BE_CPU_AGGREGATOR_HPP
