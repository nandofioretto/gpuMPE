#include <string>
#include <sstream>
#include <rapidxml.hpp>
#include <iterator>
#include <vector>
#include "string_utils.hpp"

#include "ConstraintFactory.hpp"
#include "Assert.hpp"
#include "Types.hpp"
#include "Agent.hpp"
#include "Variable.hpp"
#include "TableConstraint.hpp"
#include "Utils/Permutation.hpp" // used in UAI parsing

using namespace rapidxml;
using namespace misc_utils;
using namespace std;

// Initializes static members
int ConstraintFactory::constraintsCt = 0;

// It constructs and returns a new constraint.
Constraint::ptr ConstraintFactory::create(xml_node<>* conXML,
xml_node<>* relsXML,
vector<Agent::ptr> agents,
vector<Variable::ptr> variables)
{
	string name = conXML->first_attribute("name")->value();
	string rel = conXML->first_attribute("reference")->value();

	// Retrieve the relation associated to this constraint:
	int size = atoi(relsXML->first_attribute("nbRelations")->value());
	ASSERT(size > 0, "No Relation associated to constraint " << name);

	xml_node<>* relXML = relsXML->first_node("relation");
	while (rel.compare(relXML->first_attribute("name")->value()) != 0)
	{
		relXML = relXML->next_sibling();
		ASSERT(relXML, "No Relation associated to constraint " << name);
	}

	// Proces constraints according to their type.
	string semantics = relXML->first_attribute("semantics")->value();

	auto c = createTableConstraint(conXML, relXML, variables);
	setProperties(c, name, agents);
	++constraintsCt;
	return c;
}

// Jul 5, ok
void ConstraintFactory::setProperties
	(Constraint::ptr c, string name, vector<Agent::ptr> agents)
{
	c->setID(constraintsCt);
	c->setName(name);

	// Registers this constraint in the agents owning the variables of the 
	// constraint scope.
	for (auto v : c->getScope())
	{
		Agent::ptr v_owner = nullptr;
		for (auto a : agents) if( a->getID() == v->getAgtID() ) { v_owner = a; }
		ASSERT(v_owner, "Error in finding variable owner\n");
		v_owner->addConstraint( c );
	}
}


// Jul 5, ok
std::vector<Variable::ptr> ConstraintFactory::getScope
	(xml_node<>* conXML, std::vector<Variable::ptr> variables)
{
	int arity = atoi(conXML->first_attribute("arity")->value());
  
	// Read constraint scope
	string p_scope = conXML->first_attribute("scope")->value();
	std::vector<Variable::ptr> scope(arity, nullptr);
	stringstream ss(p_scope); int c = 0; string var;
	while( c < arity ) 
	{
		ss >> var; 
		Variable::ptr v_target = nullptr;
		for (auto& v : variables) 
			if( v->getName().compare(var) == 0 ) 
				v_target = v;
		ASSERT(v_target, "Error in retrieving scope of constraint\n");
     
		scope[ c++ ] = v_target;
	}
  
	return scope;
}


// Jul 5, ok
TableConstraint::ptr ConstraintFactory::createTableConstraint
	(xml_node<>* conXML, xml_node<>* relXML, 
std::vector<Variable::ptr> variables)
{
	// Read Relation Properties
	string name = relXML->first_attribute("name")->value();
	int arity = atoi( relXML->first_attribute("arity")->value() );
	size_t nb_tuples = atoi( relXML->first_attribute("nbTuples")->value() );
	ASSERT( nb_tuples > 0, "Extensional Soft Constraint " << name << " is empty");

	// Okey works
	std::vector<Variable::ptr> scope = getScope( conXML, variables );

	// Get the default cost
	util_t def_cost = Constants::worstvalue;

	if (relXML->first_attribute("defaultCost"))
	{
		string cost = relXML->first_attribute("defaultCost")->value();
		if (cost.compare("infinity") == 0 )
			def_cost = Constants::inf;
		else if( cost.compare("-infinity") == 0 )
			def_cost = -Constants::inf;
		else
			def_cost = std::stod(cost);
	}

	auto con = std::make_shared<TableConstraint>(scope, def_cost);

	string content = relXML->value();
	size_t lhs = 0, rhs = 0;

	// replace all the occurrences of 'infinity' with a 'INFTY'
	while (true)
	{
		rhs = content.find("infinity", lhs);
		if (rhs != string::npos)
			content.replace( rhs, 8, to_string(Constants::inf) );
		else break;
	};

	// replace all the occurrences of ':' with a '\b'
	// cost_t best_bound = Constants::worstvalue;
	// cost_t worst_bound = Constants::bestvalue;

	util_t m_cost; bool multiple_cost;
	int* tuple = new int[ arity ];
	int trim_s, trim_e;
	size_t count = 0;
	string str_tuples;
	lhs = 0;
	while (count < nb_tuples)
	{
		//multiple_cost = true;
		rhs = content.find(":", lhs);
		if (rhs < content.find("|", lhs))
		{
			if (rhs != string::npos)
			{
				m_cost = atoi( content.substr(lhs,  rhs).c_str() );

				// Keep track of the best/worst bounds
				// best_bound = Utils::getBest(m_cost, best_bound);
				// worst_bound = Utils::getWorst(m_cost, worst_bound);

				lhs = rhs + 1;
			}
		}

		rhs = content.find("|", lhs);
		trim_s = lhs, trim_e = rhs;
		lhs = trim_e+1;

		if (trim_e == string::npos) trim_e = content.size();
		else while (content[ trim_e-1 ] == ' ') trim_e--;

		str_tuples = content.substr( trim_s, trim_e - trim_s );
		str_tuples = strutils::rtrim(str_tuples);
		stringstream ss( str_tuples );

		//int tmp;
		while( ss.good() )
		{
			for (int i = 0; i < arity; ++i) {
				// ss >> tmp;
				// tuple[ i ] = scope[ i ]->getDomain().get_pos( tmp );
				ss >> tuple[ i ];
			}
			std::vector<value_t> v(tuple, tuple + arity);
			con->setUtil( v, m_cost );
			count++;
		}
	}

	// con->setBestCost(best_bound);
	// con->setWorstCost(worst_bound);

	delete[] tuple;

	return con;
}



// For WCSPs
Constraint::ptr 
	ConstraintFactory::create(size_t nTuples, util_t defCost, util_t ub,
string content,
vector<int> scopeIDs,
vector<Agent::ptr> agents,
vector<Variable::ptr> variables)
{
	vector<Variable::ptr> scope = getScope(scopeIDs, variables);
	auto con = make_shared<TableConstraint>(scope, defCost);
  
	int arity = scopeIDs.size();
	util_t util;
	std::stringstream data(content);

	// Read tuples
	for (int l=0; l<nTuples; l++) { // lines
		std::vector<value_t> tuple(arity);
    
		for (int i=0; i<arity; i++) 
			data >> tuple[i];
		data >> util;
		// if (util >= ub) util = Constants::unsat;
		con->setUtil(tuple, util);
	}
  
	string name = "c_";
	for (auto id : scopeIDs) name += to_string(id);
	setProperties(con, name, agents);
	++constraintsCt;
	return con;

}


// For UAI
Constraint::ptr ConstraintFactory::create(std::vector<util_t> entries,
                                          vector<int> scopeIDs,
                                          vector<Agent::ptr> agents,
                                          vector<Variable::ptr> variables)
{
  vector<Variable::ptr> scope = getScope(scopeIDs, variables);
  auto con = make_shared<TableConstraint>(scope, 0);
  
  int arity = scopeIDs.size();
  util_t util;
  
  std::vector<value_t> domMax;  
  for (auto v : scope) {
    domMax.push_back(v->getMax());
  }

  combinatorics::Permutation<value_t> perm(domMax);
  ASSERT(entries.size() == perm.size(), "Probability values and scope size differ: " 
         << entries.size() << " / " << perm.size() << " constr NO: " << constraintsCt );
    
  for (int e = 0; e < entries.size(); e++) { // entries
    auto tuple = perm.getPermutations()[e];
    con->setUtil(tuple, entries[e]);
  }
  
  string name = "c_";
  for (auto id : scopeIDs) name += to_string(id);
  setProperties(con, name, agents);
  ++constraintsCt;
  return con;
}


std::vector<Variable::ptr> 
	ConstraintFactory::getScope(vector<int> scopeIDs, 
vector<Variable::ptr> variables) 
{
	vector<Variable::ptr> ret;
	for (auto id : scopeIDs) {
		for (auto vptr : variables) {
			if (vptr->getAgtID() == id) { 
				ret.push_back(vptr);
				break;
			}
		}
	}
	return ret;
}

