#include <algorithm>
#include <memory>
#include <iostream>
#include <vector>
#include <utility>      // std::pair

#include "BE/CpuTable.hpp"
#include "Agent.hpp"
#include "Constraint.hpp"
#include "Variable.hpp"
#include "Types.hpp"
#include "Utils/Permutation.hpp"
#include "Utils/CpuInfo.hpp"


CpuTable::CpuTable(std::vector<Variable::ptr> vars) 
  : scope(vars) 
  //  : CpuTable(varsToAgts(vars))
{ }

CpuTable::~CpuTable() {
  //Cpu::Info::decrGlobalMemory(getSizeBytes());
  //   std::cout << "Destroying CPUTable: " << 
  //     scope.back()->getAgtID() << "\n";
}

// Create the CPU Table Associated to the Constraint con
CpuTable::CpuTable(Constraint::ptr con) {
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
  CpuTable::scope.resize(arity);

  size_t tableSize = 1;
  for (int i=0; i<arity; i++) {
    newScope[i] = prIdxStore[i].second;
    // Get bounds (in the new order)
    domMax[i]   = con->getVariableAt(newScope[i])->getMax();
    CpuTable::scope[i] = con->getVariableAt(newScope[i]);
 
    tableSize *= domMax[i];
  }
  tableSize *= arity;
  //Cpu::Info::incrGlobalMemory(tableSize*sizeof(value_t));
  
  // Gen Permutations
  combinatorics::Permutation<value_t> perm(domMax);
  // Store Information
  std::swap(CpuTable::values, perm.getPermutations());
  std::vector<value_t> oriOrderValues(arity);
  
  CpuTable::utils.resize(CpuTable::getSize(), ZERO);
  
  // For each permutation transform back the number and get value
  for (int i=0; i<CpuTable::getSize(); i++) {
    for (int j=0; j<arity; j++) {
      oriOrderValues[newScope[j]] = CpuTable::values[i][j];
    }
    // save value, and util
    utils[i] = con->getUtil(oriOrderValues);
  }
}

// Creates an Empty CPU Table Associaed to the set of variables given
CpuTable::CpuTable(std::vector<Agent::ptr> agts) {
  std::vector<std::pair<int, int>> prIdxStore;
  //     auto& v_i = agts[i]->getVariable();    
  for (int i=0; i<agts.size(); i++) {
    auto pair = std::make_pair<>(agts[i]->getPriority(), i);
    prIdxStore.push_back(pair);
  }
  std::sort(prIdxStore.begin(), prIdxStore.end(), 
	    std::less<std::pair<int,int>>());
  
  // New Ordered Constraint Scope (descending order)
  CpuTable::scope.resize(agts.size());
  std::vector<value_t> domMax(agts.size());
  
  size_t tableSize = 1;
  for (int i=0; i<agts.size(); i++) {
    int idx = prIdxStore[i].second;
    CpuTable::scope[i] = agts[idx]->getVariable();
    // Get bounds (in the new order)
    domMax[i]   = CpuTable::scope[i]->getMax();
    tableSize *= domMax[i];
  }
  tableSize *= agts.size();
  //Cpu::Info::incrGlobalMemory(tableSize*sizeof(value_t));
  
  // Gen Permutations
  combinatorics::Permutation<value_t> perm(domMax);
  // Store Information
  std::swap(CpuTable::values, perm.getPermutations());
  CpuTable::utils.resize(CpuTable::getSize(), ZERO);
}
