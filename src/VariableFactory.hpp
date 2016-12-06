#ifndef CUDA_DBE_VARIABLE_FACTORY_HPP
#define CUDA_DBE_VARIABLE_FACTORY_HPP

#include <string>
#include <vector>
#include <memory>
#include <rapidxml.hpp>

#include "Types.hpp"
#include "Variable.hpp"
#include "Agent.hpp"

// The Variable Factory class. 
// It is used to create a new variable from input.
// TODO: Generalize this class to handle any variable type (e.g., floats)
class VariableFactory
{
public:
  // It constructs and returns a new variable.
  // It also construct a new domain to be associated to the new variable.
  static Variable::ptr create(rapidxml::xml_node<>* varXML, 
			      rapidxml::xml_node<>* domsXML, 
			      std::vector<Agent::ptr> agents);
  
  // It constructs and returns a new variable, given its name, it's agent
  // owner, and its domain.
  static Variable::ptr create(std::string name, Agent::ptr owner, 
			      std::vector<value_t> dom);

  // It constructs and returns a new variable, given its name, it's agent
  // owner, and its domain bounds.
  static Variable::ptr create(std::string name, Agent::ptr owner, 
			      value_t min, value_t max);
  
  // It resets the variables counter.
  static void resetCt()
    { variablesCt = 0; }

private:
  // The variable counter. It holds the ID of the next variable to be created.
  static int variablesCt;
};

#endif // CUDA_DBE_VARIABLE_FACTORY_HPP
