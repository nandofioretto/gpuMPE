#ifndef CUDA_BE_CPU_PROJECTOR_HPP
#define CUDA_BE_CPU_PROJECTOR_HPP

#include <memory>
#include <utility>

#include "CpuTable.hpp"
#include "Variable.hpp"

using namespace misc_utils;

// Done as if we were projecting out the last variable of the table 
class CpuProjector {
public:
  // Out Table scope >= In Table scope
  static CpuTable::ptr project(const CpuTable::ptr& out, const Variable::ptr& var);

  static std::pair<value_t, util_t> project(const CpuTable::ptr& in);

};

#endif // CUDA_BE_CPU_PROJECTOR_HPP
