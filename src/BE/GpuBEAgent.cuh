//
// Created by Ferdinando Fioretto on 11/3/15.
//

#ifndef CUDA_DBE_GPUBEAGENT_CUH
#define CUDA_DBE_GPUBEAGENT_CUH

#include <vector>
#include <memory>
#include <string>

#include "Types.hpp"
#include "Agent.hpp"
#include "BE/GpuTable.cuh"


class GpuBEAgent : public Agent {

 public:
  typedef std::shared_ptr<GpuBEAgent> ptr;
  
  GpuBEAgent() {
  }

  GpuBEAgent(int _ID, std::string _name) {
    Agent::ID = _ID;
    Agent::name = _name;
  }
  
  // It fills the Agent UtilTable
  virtual void initialize();

  virtual void utilPhaseAggr();

  virtual void utilPhaseProj();

  virtual void valuePhase();
  
  GpuTable::ptr& getTable() {
    return joinedTable;
  }

private:
  // Constraints, transformed to a table
  std::shared_ptr<GpuTable> joinedTable;
  
};


#endif //
