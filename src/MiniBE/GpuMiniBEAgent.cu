//
// Created by Ferdinando Fioretto on 11/3/15.
//

#include <algorithm>
#include <vector>
#include <utils.hpp>

#include "Assert.hpp"
#include "Types.hpp"
#include "Preferences.hpp"
#include "MiniBE/GpuMiniBEAgent.cuh"
#include "BE/GpuTable.cuh"
#include "BE/GpuAggregator.cuh"
#include "BE/GpuProjector.cuh"
#include "Utils/Transform.hpp"
//#define VERBOSE

using namespace misc_utils;

std::vector<Agent::ptr> varsToAgts(std::vector<Variable::ptr> vars) {
  std::vector<Agent::ptr> agts;
  for (auto& v : vars){ 
    agts.push_back(v->getAgt());
  }
  return agts;
}

/// Initialize wont' be able to be used here
void GpuMiniBEAgent::initialize() {
  if (Agent::isLeaf())
    utilPhaseInit();
}

void GpuMiniBEAgent::utilPhaseInit() {
  // Create Tables to Merge where we list agent x_i as first element of the table
  // The order of the scope of the constraints follows that of the pseudo-tree, 
  // where leaves come first and root last 
  // I only need to retrieve the Constraints with ancestors!
  for (auto& con : getAncestorsConstraints()) {
    bucket.push_back(std::make_shared<GpuTable>(con));
  }
 
  for (auto& con : getUnaryConstraints()) {
    bucket.push_back(std::make_shared<GpuTable>(con));
  }
 
  // Create miniBucket Tables which contains all permutations in the 
  // scope of the minibuckets 
  while (!bucket.empty()) {
    std::vector<Variable::ptr> vars;
    std::vector<GpuTable::ptr> miniBucketTmp;

    auto itab = bucket.begin();
    while (itab != bucket.end()) {
      if (utils::merge(vars, (*itab)->getScope()).size() <= miniBucketSize) {
        utils::merge_emplace(vars, (*itab)->getScope());
        miniBucketTmp.push_back(*itab);
        bucket.erase(itab);
      }
      else {
        itab++;
      }
    }

    Assert::check(!vars.empty(), 
                  "MiniBucket maximal size chosen insufficient", "MBL");
    
    // Will Sort variables so that lower priority (this agent) appears as last
    auto tab = std::make_shared<GpuTable>(varsToAgts(vars));

    // Include all Constraints in cpuTables in the above table
    //   - Integrate cost Tables
    for (auto it : miniBucketTmp) {
      Gpu::Aggregator::join(tab, it);
    }
    miniBuckets.push_back(tab);
  }
  
  // !! TODO (GPU): Make sure that the tables in bucket are destroied -- destructor is called.
  bucket.clear();
}


void GpuMiniBEAgent::utilPhaseAggr() {
  if (!Agent::isLeaf()) {
    utilPhaseInit();
  }
}


void GpuMiniBEAgent::utilPhaseProj() {
  if (!Agent::isRoot()) {

    // Copy temp bucket to retrieve information later.
    bucket = miniBuckets;

    // This assignment causes the previous table to be freed.
    for (auto& miniBucket : miniBuckets) {
      
      miniBucket = Gpu::Projector::project(miniBucket, Agent::getVariable());
      // Send Message containing reduced bucket to ancestor listed as last
      // in the scope of the projected table.
      auto destAgt = miniBucket->getScope().back()->getAgt();
      auto miniBEdestAgt = std::dynamic_pointer_cast<GpuMiniBEAgent>(destAgt);
      miniBEdestAgt->insertBucket(miniBucket);
    }
  }
}



// [Todo-Fix] This is wrong: 
// Other than computing this cost you also need to add it to the row of the 
// table wich match with the current separator variables.
void GpuMiniBEAgent::valuePhase() {
  if (Agent::isRoot()) {
    ASSERT(miniBuckets.size() == 1, "multie buckets found at the root.");
    
    auto pair = Gpu::Projector::project(miniBuckets.front());
    Agent::setUtil(pair.second);
    Agent::getVariable()->setValue(pair.first);

  } 
#ifdef FASLE
  else {
    // For every value of the domain of the variable of this ageint, 
    // Explore all constraints subjected to this values, and pick
    // the value domain which maximizes such cost.
    int bestVal = 0;
    util_t bestUtil = Constants::worstvalue;
    auto& var = Agent::getVariable();

    for (int d=var->getMin(); d<=var->getMax(); d++) {
      util_t sumUtil = 0;
      
      // NOTE: We have cleared the bucket array in an earlier function!
      //       This part is wrong - you'd need to mirror the bucket array 
      //       onto the host and then do this operation.
      for (auto& tab : bucket) {
        // build index:
        size_t idx = 0;
        auto scope = tab->getScope();

        idx = scope[0]->getValue();
        for(int i=1; i<scope.size()-1; i++) {
          idx *= scope[i]->getDomSize(); 
          idx += scope[i]->getValue();
        }
        idx += d;
        sumUtil += tab->getUtil(idx);
      }

      if (Preferences::verbose) {
        //----------------
        std::cout << Agent::getName() << "[" << d <<"] util: " << sumUtil << "\n";
        //----------------
      }

      if (sumUtil OP bestUtil) {
	bestUtil = sumUtil;
	bestVal = d;
      }
    }//- for all d in D
    
    //Agent::setUtil(bestUtil);
    var->setValue(bestVal);
  }
#endif
}
