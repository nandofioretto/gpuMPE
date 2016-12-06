//
// Created by Ferdinando Fioretto on 11/3/15.
//

#include <algorithm>
#include <vector>
#include <utils.hpp>

#include "Assert.hpp"
#include "Types.hpp"
#include "Preferences.hpp"
#include "BE/CpuBEAgent.hpp"
#include "BE/CpuTable.hpp"
#include "BE/CpuAggregator.hpp"
#include "BE/CpuProjector.hpp"

//#define VERBOSE

using namespace misc_utils;

void CpuBEAgent::initialize() {
  // Create Tables to Merge where we list agent x_i as first element of the table
  // The order of the scope of the constraints follows that of the pseudo-tree, 
  // where leaves come first and root last 
  std::vector<std::shared_ptr<CpuTable>> cpuTables;

  // Only need to retrieve the Constraints with ancestors!
  for (auto& con : getAncestorsConstraints()) {
    cpuTables.push_back(std::make_shared<CpuTable>(con));
  }

  // Get Unary Constraints
  for (auto& con : getUnaryConstraints()) {
    cpuTables.push_back(std::make_shared<CpuTable>(con));
  }

  // Create an empty (CPU) or (GPU) Table which contains all permutations in the 
  // Separator Set of this agent  
  auto V = misc_utils::utils::concat(Agent::getSeparator(), Agent::getPtr());
  joinedTable = std::make_shared<CpuTable>(V);
  
  // Include all Constraints in cpuTables in the above table
  //   - Integrate cost Tables
  for (auto& table : cpuTables) {
    CpuAggregator::join(joinedTable, table);	
  }

}


void CpuBEAgent::utilPhaseAggr() {
  for (auto& agt : Agent::getChildren()) {
    auto BEagt = std::dynamic_pointer_cast<CpuBEAgent>(agt);
    auto chTable = BEagt->getTable();
    CpuAggregator::join(joinedTable, chTable);
  }
}


void CpuBEAgent::utilPhaseProj() {
  if (!Agent::isRoot()) {
    // This assignment causes the previous table to be freed.
    joinedTable = CpuProjector::project(joinedTable, Agent::getVariable());
  }
}



// [Todo-Fix] This is wrong: 
// Other than computing this cost you also need to add it to the row of the 
// table wich match with the current separator variables.
void CpuBEAgent::valuePhase() {
  if (Agent::isRoot()) {
    auto pair = CpuProjector::project(joinedTable);
    Agent::setUtil(pair.second);
    Agent::getVariable()->setValue(pair.first);
  }
  else {
    auto ancestors = utils::concat(Agent::getParent(), Agent::getPseudoParents());
    std::sort(ancestors.begin(), ancestors.end(), Agent::orderLt);    
    
    // For every value of the domain of the variable of this ageint, 
    // Explore all constraints subjected to this values, and pick
    // the value domain which maximizes such cost.
    int bestVal = 0;
    util_t bestUtil = Constants::worstvalue;
    auto& var = Agent::getVariable();

    for (int d=var->getMin(); d<var->getMax(); d++) {
      util_t sumUtil = 0;
      for (auto& con : getAncestorsConstraints()) {
	std::vector<value_t> tuple(con->getArity(), -1);
	int i = 0;
	for (auto& v : con->getScope()) {
	  tuple[i++] = v->getAgtID() == Agent::getID() ? d : v->getValue();
	}
	sumUtil = sumUtil JoinOP con->getUtil(tuple);
      }//- for all c in ancestor's constraints

#ifndef MPE
      if (sumUtil RelOP bestUtil)
        bestVal = d;
#endif
      bestUtil = ProjOP(bestUtil, sumUtil);

    }//- for all d in D
    
    //Agent::setUtil(bestUtil);
    Agent::getVariable()->setValue(bestVal);
  }

}
