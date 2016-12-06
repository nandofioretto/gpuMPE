#include <iostream>
#include <algorithm>

#include <string_utils.hpp>

#include "Preferences.hpp"
#include "Problem.hpp"
#include "BE/BESolver.hpp"
#include "BE/CpuBEAgent.hpp"
#include "BE/GpuBEAgent.cuh"
#include "MiniBE/CpuMiniBEAgent.hpp"
#include "Types.hpp"

#include "Utils/Timer.hpp"

using namespace misc_utils;

void BESolver::solve() {

  auto agents = Problem::getAgents();
  std::sort(agents.begin(), agents.end(), Agent::orderGt);
  
  for (auto agt : agents) {
    if (Preferences::verbose) {
      std::cout << "Agent " << agt->getName() << " Aggregate\n";
    }
    timerUtilPhaseAggr[agt->getID()].start();
    agt->utilPhaseAggr();
    timerUtilPhaseAggr[agt->getID()].pause();    

    if (Preferences::verbose) {
      std::cout << "Agent " << agt->getName() << " Project\n";
    }
    timerUtilPhaseProj[agt->getID()].start();
    agt->utilPhaseProj();
    timerUtilPhaseProj[agt->getID()].pause();    
  }
  
  sort(agents.begin(), agents.end(), Agent::orderLt);
  for (auto agt : agents) {
    if (Preferences::verbose) {
      std::cout << "Agent " << agt->getName() << " Value\n";
    }
    timerValuePhase[agt->getID()].start();
    agt->valuePhase();
    timerValuePhase[agt->getID()].pause();    
  }
  
}
