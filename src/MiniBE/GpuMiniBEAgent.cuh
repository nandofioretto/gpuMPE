//
// Created by Ferdinando Fioretto on 21/5/15.
//

#ifndef CUDA_DBE_MINIBEAGENT_CUH
#define CUDA_DBE_MINIBEAGENT_CUH

#include <vector>
#include <memory>
#include <string>

#include "Types.hpp"
#include "Agent.hpp"
#include "BE/GpuTable.cuh"


class GpuMiniBEAgent : public Agent {

 public:
  typedef std::shared_ptr<GpuMiniBEAgent> ptr;
  
  // z = maximum
  GpuMiniBEAgent(int z) 
    : miniBucketSize(z)
  { }

  GpuMiniBEAgent(int z, int _ID, std::string _name) 
    : miniBucketSize(z) {
    Agent::ID = _ID;
    Agent::name = _name;
  }
  
  // It fills the Agent UtilTable
  virtual void initialize();

  virtual void utilPhaseInit();

  virtual void utilPhaseAggr();

  virtual void utilPhaseProj();

  virtual void valuePhase();
  
  // CpuTable::ptr& getTable() {
  //   return joinedTable;
  // }

  void insertBucket(GpuTable::ptr tab) {
    // std::cout << "received table: " << tab->to_string();
    bucket.push_back(tab);
  }

private:
  // The bucket contains the tables received by the descendant agents
  // and the functions whose scope include this agent. 
  // The bucket will be partitioned into minibuckets, and then deleted.
  std::vector<GpuTable::ptr> bucket;

  // Max size for the scope of each variable in the minibucket.
  int miniBucketSize;

  // The partition of the bucket - already merged into single tables
  std::vector<GpuTable::ptr> miniBuckets;
  
};


#endif //
