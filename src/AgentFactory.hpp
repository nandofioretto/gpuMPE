#ifndef CUDA_DBE_AGENT_FACTORY_HPP
#define CUDA_DBE_AGENT_FACTORY_HPP

#include <string>
#include <memory>

#include "Assert.hpp"
#include "Agent.hpp"
#include "BE/CpuBEAgent.hpp"
#include "BE/GpuBEAgent.cuh"
#include "MiniBE/CpuMiniBEAgent.hpp"
#include "MiniBE/GpuMiniBEAgent.cuh"
#include "InputParams.hpp"

// The Agent Factory class.
class AgentFactory
{
public:
  // Construct and returns a new agent, given its name.
  static Agent::ptr create(std::string name, InputParams::agent_t type,
                           int z=0) {
    ASSERT(!name.empty(), "Error: agent name cannot be empty");
    
    switch (type) {
    case InputParams::cpuBE:
      return std::make_shared<CpuBEAgent>(agentsCt++, name);
    case InputParams::gpuBE:
      return std::make_shared<GpuBEAgent>(agentsCt++, name);
    case InputParams::cpuMiniBE:
      return std::make_shared<CpuMiniBEAgent>(z, agentsCt++, name);
    case InputParams::gpuMiniBE:
      return std::make_shared<GpuMiniBEAgent>(z, agentsCt++, name);
    }
    return nullptr;
  }
  
  // It resets the agents counter.
  static void resetCt() 
    { agentsCt = 0; }

private:
  // The Agent counter. It holds the ID of the next agent to be created.
  static int agentsCt;
};

#endif
