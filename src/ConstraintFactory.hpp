#ifndef CUDA_DBE_CONSTRAINT_FACTORY_HPP 
#define CUDA_DBE_CONSTRAINT_FACTORY_HPP

#include <rapidxml.hpp>
#include <string>
#include <vector>
#include <memory>

#include "Types.hpp"
#include "Agent.hpp"
#include "Variable.hpp"
#include "Constraint.hpp"
#include "TableConstraint.hpp"


class ConstraintFactory
{
public:
  
	// It constructs and returns a new constraint.
	// XML
	static Constraint::ptr create(rapidxml::xml_node<>* conXML, 
                                      rapidxml::xml_node<>* relsXML,
                                      std::vector<Agent::ptr> agents,
                                      std::vector<Variable::ptr> variables);
	// XCSP
	static Constraint::ptr create(size_t nTuples, util_t defCost, util_t ub,
                                      std::string content,
                                      std::vector<int> scopeIds,
                                      std::vector<Agent::ptr> agents,
                                      std::vector<Variable::ptr> variables);
  
    // UAI
	static Constraint::ptr create(std::vector<util_t> entries,
		std::vector<int> scopeIDs,
		std::vector<Agent::ptr> agents,
		std::vector<Variable::ptr> variables);

        // It resets the constraints counter.
          static void resetCt() { constraintsCt = 0; }

private:
  
	// The Constraints counter. It holds the ID of the next constraint to be
	// created.
	static int constraintsCt;

	// It returns the scope of the constraint 
	// XML
	static std::vector<Variable::ptr> getScope
		(rapidxml::xml_node<>* conXML, std::vector<Variable::ptr> variables);

	// WCSP
	static std::vector<Variable::ptr> getScope(std::vector<int> scopeIDs, 
	std::vector<Variable::ptr> variables);
  
	// It constructs an extensional hard constraint from the xml bit
	static TableConstraint::ptr createTableConstraint
		(rapidxml::xml_node<>* conXML, rapidxml::xml_node<>* relXML, 
	std::vector<Variable::ptr> variables);

	// Sets common constraint properties and initializes mappings.
	static void setProperties(Constraint::ptr c, std::string name, 
	std::vector<Agent::ptr> agents);

};


#endif // ULYSSES_KERNEL__CONSTRAINTS__CONSTRAINT_FACTORY_H_
