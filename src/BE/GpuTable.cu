#include <algorithm>
#include <cmath>
#include <memory>
#include <iostream>
#include <vector>
#include <utility>      // std::pair
#include <cuda.h>
#include <cuda_runtime.h>

#include "Assert.cuh"
#include "Agent.hpp"
#include "Constraint.hpp"
#include "Variable.hpp"
#include "Types.hpp"
#include "Utils/Permutation.hpp"
#include "Utils/GpuAllocator.cuh"
#include "BE/GpuTable.cuh"

// Create the CPU Table Associated to the Constraint con
GpuTable::GpuTable(Constraint::ptr con)
  : values(nullptr), utils(nullptr), nbEntries(0)
{
  std::vector<std::pair<int, int>> prIdxStore;
  int arity = con->getArity();
  
  for (int i=0; i<arity; i++) {
    auto& agt_i = con->getVariableAt(i)->getAgt();
    auto pair = std::make_pair<>(agt_i->getPriority(), i);
    prIdxStore.push_back(pair);
  }
  std::sort(prIdxStore.begin(), prIdxStore.end(), 
	    std::less<std::pair<int,int>>());
  
  // New Ordered Constraint Scope (acending order) - variable to eliminate 
  // appear as last.
  std::vector<int> newScope(arity);
  std::vector<value_t> domMax(arity);
  GpuTable::scope.resize(arity);
  
  for (int i=0; i<arity; i++) {
    newScope[i] = prIdxStore[i].second;
    // Get bounds (in the new order)
    domMax[i]   = con->getVariableAt(newScope[i])->getMax();
    GpuTable::scope[i] = con->getVariableAt(newScope[i]);
  }
  
  // Gen Permutations
  combinatorics::Permutation<value_t> perm(domMax);
  // Store Information
  GpuTable::nbEntries = perm.size();
  auto& permList = perm.getPermutations();
  std::vector<util_t> hostUtils(GpuTable::nbEntries, ZERO);
  std::vector<value_t> oriOrderValues(arity);
   
  // For each permutation transform back the number and get value
  for (int i=0; i<GpuTable::nbEntries; i++) {
    for (int j=0; j<arity; j++) {
      oriOrderValues[newScope[j]] = permList[i][j];
    }
    // save value, and util
    hostUtils[i] = con->getUtil(oriOrderValues);
  }

  // Allocate data on Device and save pointer back to GpuTable
  GpuTable::utils = Gpu::Allocator::alloc(hostUtils);
}



// Creates an Empty GPU Table Associaed to the set of variables given
// [todo] This step is not needed (do not need to generate all perm to count them!
GpuTable::GpuTable(std::vector<Agent::ptr> agts) 
  : values(nullptr), utils(nullptr), nbEntries(0)
{
  std::vector<std::pair<int, int>> prIdxStore;
  //     auto& v_i = agts[i]->getVariable();    
  for (int i=0; i<agts.size(); i++) {
    auto pair = std::make_pair<>(agts[i]->getPriority(), i);
    prIdxStore.push_back(pair);
  }
  std::sort(prIdxStore.begin(), prIdxStore.end(), 
	    std::less<std::pair<int,int>>());
  
  // New Ordered Constraint Scope (descending order)
  GpuTable::scope.resize(agts.size());

  GpuTable::nbEntries = 1;
  for (int i=0; i<agts.size(); i++) {
    int idx = prIdxStore[i].second;
    GpuTable::scope[i] = agts[idx]->getVariable();
    // Get bounds (in the new order)
    GpuTable::nbEntries *= GpuTable::scope[i]->getDomSize();
  }

  // Store Information
  GpuTable::utils = Gpu::Allocator::alloc(nbEntries, ZERO);  
}


GpuTable::~GpuTable() {
  if (values)
    Gpu::Allocator::free(values);
  if (utils)
    Gpu::Allocator::free(utils);
}



std::string 
GpuTable::to_string() {
    std::string res;
    res += " scope = [";
    for (auto v : scope) res += v->getName() + ",";
    res += "]\n";

    util_t *h_utils = (util_t*) malloc(nbEntries * sizeof(util_t));
    cuCheck(cudaMemcpy(h_utils, utils, sizeof(util_t) * nbEntries, cudaMemcpyDeviceToHost));
    
    for (int i=0; i<std::min((size_t)10, nbEntries); i++) {
      res += std::to_string(i) + ": " + std::to_string(h_utils[i]) + "\n"; 
    }

    if (h_utils) free(h_utils);
    return res;
  }


void 
GpuTable::update(std::vector<util_t> hostUtils, std::vector<Variable::ptr> vars) {
  size_t newSize = hostUtils.size();
  if (newSize > nbEntries) {
    Gpu::Allocator::free(utils);
    utils = Gpu::Allocator::alloc(hostUtils);
  } else {
    Gpu::Allocator::cpyToDevice(utils, &hostUtils[0], newSize);
  }
  nbEntries = newSize;
  scope = vars;
}
 
