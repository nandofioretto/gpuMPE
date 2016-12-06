//
// Created by Ferdinando Fioretto on 10/31/15.
//

#ifndef CUDA_DBE_PROBLEM_HPP
#define CUDA_DBE_PROBLEM_HPP

#include <memory>
#include <map>
#include <cmath>
#include <string>
#include <vector>
#include <rapidxml.hpp>

#include "Agent.hpp"
#include "Constraint.hpp"
#include "InputParams.hpp"
#include "Preferences.hpp"
#include "Utils/CpuInfo.hpp"
#include "Utils/GpuInfo.cuh"

class Problem {
public:
  typedef std::shared_ptr<Problem> ptr;

  static void parse(std::string fileName, InputParams::format_t format,
		    InputParams::agent_t agt) {
      switch(format) {
      case InputParams::xml:
        if (!Preferences::silent) {
          std::cout << "Importing file XML..."; 
          std::flush(std::cout);
	}
        importXML(fileName, agt);
	break;
      case InputParams::wcsp:
        if (!Preferences::silent) {
          std::cout << "Iporting file WCSP...";	
          std::flush(std::cout);
        }
        importWCSP(fileName, agt);
	break;
      case InputParams::uai:
        if (!Preferences::silent) {
          std::cout << "Importing file UAI...";
          std::flush(std::cout);
	}
        importUAI(fileName, agt);
	break;
      }
      makeMaps();

      if (!Preferences::silent)
        std::cout << "\tok\n";
    }

    static void importXML(std::string fileName, InputParams::agent_t agt);
 
    static void importWCSP(std::string fileName, InputParams::agent_t agt);

    static void importUAI(std::string fileName, InputParams::agent_t agt);

    static int getNbAgents() {
      return agents.size();
    }

    static int getNbConstraints() {
      return constraints.size();
    }

    static std::vector<Agent::ptr>& getAgents() {
      return agents;
    }

    static Agent::ptr& getAgent(int ID) { return agents[mapAgents[ID]]; }

    static std::vector<std::vector<int>> getForest();

    static void makePseudoTreeOrder();

    static void makePseudoTreeOrder(int rootId, int heur=-1);
  
    static void makePseudoTreeOrder
    (std::vector<int> agentsId, int root, int heur, int level=0);
    
    static void makeLinearOrder() { }

    static int getInducedWidth();

    static int getInducedWidth(std::vector<int> tree);

    static void setAgentsPriorities(int root, int level);
    
  static bool existsSavedPseudoTree();
  static void loadPseudoTree(); 
  static void savePseudoTree(std::vector<int> roots, std::vector<int> heurs);

  static bool checkMemory() {

      int maxBucketSize = InputParams::getAgentParam();
      size_t hostMemRequired = 0;
      size_t devMemRequired = 0;

      for (auto& agt : agents) {
        size_t agtSize = agt->getVariable()->getDomSize();
        int miniBucketSize = 1;
        for (auto& sep : agt->getSeparator()) {
          agtSize *= sep->getVariable()->getDomSize();

          // MiniBuckets Size
          if (miniBucketSize++ == maxBucketSize) {
            miniBucketSize = 1;
            hostMemRequired += (agtSize * sizeof(value_t));
            devMemRequired = std::max(devMemRequired, agtSize * sizeof(value_t));
            agtSize = agt->getVariable()->getDomSize();            
          }
        }

        // Normal Bucket Size
        if (InputParams::getAgentParam() == 0)  {
            hostMemRequired += (agtSize * sizeof(value_t));
            devMemRequired = std::max(devMemRequired, agtSize * sizeof(value_t));
        }

      }

      // if aggregate GPU memory exceeds single bucket, then trigger smarter (but
      // slower copies of memory from GPU to CPU). 

      if (!Preferences::silent)
        std::cout << "Tot. estimate host memory required: " 
                  << (hostMemRequired / 1000000000.00) << " GB / Available: "
                  << Cpu::Info::getAvailableGlobalMemoryMB()/1000.00 << " GB\n" 
                  << "Min. estimate device memory required: " 
                  << (devMemRequired / 1000000000.00) << " GB / Available: "
                  << Gpu::Info::getAvailableGlobalMemoryMB()/1000.00 << " GB\n";
      
      return (hostMemRequired <= Cpu::Info::getAvailableGlobalMemoryBytes() && 
              (0 == Gpu::Info::getAvailableGlobalMemoryBytes() || 
               devMemRequired <= Gpu::Info::getAvailableGlobalMemoryBytes()) );
    }

  
  static void initialize() {
    for (auto& agt : agents)
      agt->initialize();
  }

  static std::string to_string() {
    std::string ret = "DCOP Problem\n";
    ret += "AGENTS\n";
    for (auto a : agents) 
      ret += a->to_string() + "\n";
    ret += "CONSTRAINTS\n";
    for (auto c : constraints)
      ret += c->to_string() + "\n"; 
    return ret;
  }
  
  static util_t computeUtil() {
    util_t util = 0;
    for (auto& c : constraints) {
      std::vector<value_t> vscope;
      for (auto& v : c->getScope())
	vscope.push_back(v->getValue());
      
      util += c->getUtil(vscope);
    }
    return util;
  }


private:
  static void parseXMLAgents(rapidxml::xml_node<>* root, InputParams::agent_t agt);
  static void parseXMLVariables(rapidxml::xml_node<>* root);
  static void parseXMLConstraints(rapidxml::xml_node<>* root);
  static void makeMaps() {
    const int nAgents = agents.size();
    mapAgents.resize(nAgents);
    for (int i=0; i<nAgents; i++) {
      mapAgents[ agents[i]->getID() ] = i;
    }
  }

  static bool order_des(int LHS, int RHS);
  static bool order_asc(int LHS, int RHS);
  static bool lex_asc(int LHS, int RHS);
  static bool lex_des(int LHS, int RHS);
  static bool order_neig_asc(int LHS, int RHS);
  static bool order_neig_des(int LHS, int RHS);

private:

  static std::vector<std::shared_ptr<Agent>> agents;
  static std::vector<std::shared_ptr<Variable>> variables;
  static std::vector<std::shared_ptr<Constraint>> constraints;
  static std::vector<int> mapAgents; // only one active
  static std::vector<int> mapVariables;
  static std::vector<int> mapConstraints;
};


#endif //D_AGC_DR_PROBLEM_H
