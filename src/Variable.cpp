#include "Variable.hpp"
#include "Agent.hpp"

Variable::Variable (std::vector<value_t> _values,  Agent::ptr& _agt, 
		    value_t _prior)
  : value(-1), prior(_prior), values(_values), agt(_agt), agtID(_agt->getID()) {
  
  std::sort(Variable::values.begin(), Variable::values.end());
  min = values.front();
  max = values.back();
}


Variable::Variable (value_t _min, value_t _max, Agent::ptr& _agt, value_t _prior)
  : value(-1), prior(_prior), agt(_agt), agtID(_agt->getID()) {

    Variable::min = _min;
    Variable::max = _max;
    for (int i=min; i<=max; i++)
      values.push_back(i);
}


bool Variable::orderLt (const ptr& lhs, const ptr& rhs) {
    return Agent::orderLt(lhs->getAgt(), rhs->getAgt());
}

bool Variable::orderGt (const ptr& lhs, const ptr& rhs) {
  return Agent::orderGt(lhs->getAgt(), rhs->getAgt());
}
