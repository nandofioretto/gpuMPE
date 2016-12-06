#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <rapidxml.hpp>

#include "Assert.hpp"
#include "VariableFactory.hpp"
#include "Variable.hpp"
#include "Agent.hpp"

using namespace rapidxml;

// Initializes static members
int VariableFactory::variablesCt = 0;


Variable::ptr VariableFactory::create(xml_node<>* varXML, 
                                     xml_node<>* domsXML, 
                                     std::vector<Agent::ptr> agents)
{
  // after you create the variable you can use the var id to identify it
  std::string name = varXML->first_attribute("name")->value();
  std::string ownerName = varXML->first_attribute("agent")->value();
  std::string domain = varXML->first_attribute("domain")->value();
  
  // Retrieve domain xml_node:
  xml_node<>* domXML = domsXML->first_node("domain");
  while (domain.compare(domXML->first_attribute("name")->value()) != 0)
  {
    domXML = domXML->next_sibling();
    ASSERT(domXML, 
      "No domain associated to variable " << name << " could be found.");
  }

  // look for owner in agents vector:
  Agent::ptr owner = nullptr;
  for (auto a : agents) {
    if( a->getName().compare(ownerName) == 0 )
      owner = a;
  }
  ASSERT(owner, 
    "No agent associated to variable " << name << " could be found.");
  
  std::string content = domXML->value();
  size_t ival = content.find("..");
  ASSERT (ival != std::string::npos, "Cannot handle not contiguous domains");
  int min = atoi( content.substr(0, ival).c_str() ); 
  int max = atoi( content.substr(ival+2).c_str() ); 

  return create(name, owner, min, max);
}


Variable::ptr VariableFactory::create(std::string name, Agent::ptr owner, 
				     std::vector<value_t> dom)
{

  ASSERT(owner, "No agent associated to variable " << name << " given.");
    
  Variable::ptr var = std::make_shared<Variable>(dom, owner);
  var->setName( name );
  // Register variable in the agent owning it
  owner->addVariable( var );
  
  ++variablesCt;
  return var;
}


Variable::ptr VariableFactory::create(std::string name, Agent::ptr owner, 
				      value_t min, value_t max)
{
  ASSERT(owner, "No agent associated to variable " << name << " given.");
    
  Variable::ptr var = std::make_shared<Variable>(min, max, owner);
  var->setName( name );
  // Register variable in the agent owning it
  owner->addVariable( var );
  ++variablesCt;

  return var;
}
