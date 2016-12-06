//
// Created by Ferdinando Fioretto on 11/3/15.
//

#ifndef CUDA_DBE_BEAGENT_HPP
#define CUDA_DBE_BEAGENT_HPP

#include <vector>
#include <memory>
#include <string>

#include "Types.hpp"
#include "Agent.hpp"
#include "BE/CpuTable.hpp"


class CpuBEAgent : public Agent {

 public:
  typedef std::shared_ptr<CpuBEAgent> ptr;
  
  CpuBEAgent() {
  }

  CpuBEAgent(int _ID, std::string _name) {
    Agent::ID = _ID;
    Agent::name = _name;
  }
  
  // It fills the Agent UtilTable
  virtual void initialize();

  virtual void utilPhaseAggr();

  virtual void utilPhaseProj();
  
  virtual void valuePhase();
  
  CpuTable::ptr& getTable() {
    return joinedTable;
  }

private:
  // Constraints, transformed to a table
  std::shared_ptr<CpuTable> joinedTable;
  
};


#endif //
