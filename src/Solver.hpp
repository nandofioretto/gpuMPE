//
// Created by Ferdinando Fioretto on 11/3/15.
//

#ifndef CUDA_DBE_SOLVER_H
#define CUDA_DBE_SOLVER_H

#include <string>
#include "Problem.hpp"
#include "Preferences.hpp"
#include "Utils/Timer.hpp"

class Solver {
public:
  Solver(int _maxBuckets=0)
    : maxBuckets( _maxBuckets ) 
    { 
      timerUtilPhaseAggr.resize(Problem::getNbAgents());
      timerUtilPhaseProj.resize(Problem::getNbAgents());
      timerValuePhase.resize(Problem::getNbAgents());

      Problem::initialize();
    }

    virtual void solve() = 0;

    int getMaxBuckets() const {
        return maxBuckets;
    }

	std::vector<size_t> getUtilPhaseAggrTime()  {
		std::vector<size_t> times;
		for (auto agt : Problem::getAgents()) {
			times.push_back(timerUtilPhaseAggr[agt->getID()].getElapsed());
		}
		return times;
	}

	std::vector<size_t> getUtilPhaseProjTime()  {
		std::vector<size_t> times;
		for (auto agt : Problem::getAgents()) {
			times.push_back(timerUtilPhaseProj[agt->getID()].getElapsed());
		}
		return times;
	}

	
  size_t getSimulatedTime() {
    size_t highestTime = 0;
    for (auto agt : Problem::getAgents()) {
      size_t agtTime = 
        timerUtilPhaseAggr[agt->getID()].getElapsed() 
        + timerUtilPhaseProj[agt->getID()].getElapsed()
        + timerValuePhase[agt->getID()].getElapsed();
      if (agtTime > highestTime)
        highestTime = agtTime;
    }
    return highestTime;
  }

  util_t getProblemUtil() {
    //return Problem::getAgent(Preferences::ptRoot)->getUtil();
    util_t util = 0;
    for (auto agt : Problem::getAgents()){
      if (agt->isRoot()) 
        util += agt->getUtil();
    }
    return util;
  }

  // Print Timers
  virtual std::string to_string()
  {
    std::string ret = "Problem Util = " 
      + std::to_string(Problem::getAgent(Preferences::ptRoot)->getUtil())
      + "\n";
    for (auto agt : Problem::getAgents())
      ret += agt->getName() + "\t";
    ret += "\n";
    for (auto agt : Problem::getAgents())
      ret += std::to_string((value_t)agt->getVariable()->getValue()) + "\t";
    ret += "\n";

    for (auto agt : Problem::getAgents()) {
      ret += "Agent " + agt->getName()
	+ " Util Phase (Aggr): " 
	+ std::to_string(timerUtilPhaseAggr[agt->getID()].getElapsed()) + " ms \t"
        + " Util Phase (Proj): "
	+ std::to_string(timerUtilPhaseProj[agt->getID()].getElapsed()) + " ms \t"
        + " Value Phase      : "
	+ std::to_string(timerValuePhase[agt->getID()].getElapsed()) + " ms \n";
    }
    return ret;
  }

protected:
  int maxBuckets;

  std::vector<Timer<>> timerUtilPhaseAggr;
  std::vector<Timer<>> timerUtilPhaseProj;
  std::vector<Timer<>> timerValuePhase;

};

#endif //CUDA_DBE_SOLVER_H
