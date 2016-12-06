#ifndef UTIL_TRANSFORM_HPP
#define UTIL_TRANSFORM_HPP

#include <vector>
#include <memory>

#include "Agent.hpp"
#include "Variable.hpp"

class Transform
{
public:

  static std::vector<Agent::ptr> varsToAgts(std::vector<Variable::ptr> vars) {
    std::vector<Agent::ptr> agts;
    for (auto& v : vars){ 
      agts.push_back(v->getAgt());
    }
    return agts;
  }

};

#endif
