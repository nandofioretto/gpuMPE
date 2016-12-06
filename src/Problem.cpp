//
// Created by Ferdinando Fioretto on 10/31/15.
//

#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <rapidxml.hpp>
#include <memory>
#include <algorithm>
#include <stack>
#include <queue>
#include <string>

#include "Preferences.hpp"
#include "Assert.hpp"
#include "Types.hpp"
#include "Problem.hpp"
#include "AgentFactory.hpp"
#include "VariableFactory.hpp"
#include "ConstraintFactory.hpp"

#include "utils.hpp"
#include "string_utils.hpp"

using namespace misc_utils;
using namespace rapidxml;
using namespace std;


std::vector<std::shared_ptr<Agent>> Problem::agents;;
std::vector<std::shared_ptr<Variable>> Problem::variables;
std::vector<std::shared_ptr<Constraint>> Problem::constraints;
std::vector<int> Problem::mapAgents; // only one active
std::vector<int> Problem::mapVariables;
std::vector<int> Problem::mapConstraints;



void Problem::importXML(std::string fileName, InputParams::agent_t agtType) {
	int size = 0;
	std::string input_xml;
	std::string line;
	std::ifstream in( fileName.c_str() );

	ASSERT(in.is_open(), "Error: cannot open the input file.");
  
	while (getline( in, line ))
		input_xml += line;
 
	// make a safe-to-modify copy of input_xml
	std::vector<char> xml_copy(input_xml.begin(), input_xml.end());
	xml_copy.push_back('\0');
	xml_document<> doc;
	doc.parse<parse_declaration_node | parse_no_data_nodes>( &xml_copy[ 0 ] );
	xml_node<>* root = doc.first_node("instance");
	xml_node<>* xpre = root->first_node("presentation");
  
	// Set the problem optimization type
	ASSERT(xpre->first_attribute("maximize"), 
	"Invalid optimization Problem (maximize/minimize) specified \
		in the XML file!");
  
	std::string optMax = xpre->first_attribute("maximize")->value();  
	ASSERT (optMax.compare("true") == 0, 
	"Cuda-DBE is defined only for maximization problems");

	parseXMLAgents(root, agtType);
	parseXMLVariables(root);
	parseXMLConstraints(root); 

	for (auto& a : agents) {
    
		//   a->orderContextVariables();
	}
  
	in.close();
}

void Problem::parseXMLAgents(xml_node<>* root, InputParams::agent_t agtType)
{
	xml_node<>* xagents = root->first_node("agents");
	int nb_agents = atoi( xagents->first_attribute("nbAgents")->value() );
	xml_node<>* xagent  = xagents->first_node("agent");

	int agentID = 0;
	do
	{
		std::string name = xagent->first_attribute("name")->value();
		auto agent = AgentFactory::create(name, agtType, InputParams::getAgentParam());
		//auto agent = std::make_shared<Agent> (agentID++, name);
		agents.push_back(agent);

		//std::cout << agent->to_string() << std::endl;

		xagent = xagent->next_sibling();
	} while (xagent);

	ASSERT( nb_agents == agents.size(), "Number of agents read " 
		<< agents.size() << " differs from the number of agents declared.");
}

void Problem::parseXMLVariables(xml_node<>* root)
{
	xml_node<>* xdoms = root->first_node("domains");
	xml_node<>* xvars = root->first_node("variables");
	xml_node<>* xvar = xvars->first_node("variable");
	int nb_variables = atoi( xvars->first_attribute("nbVariables")->value() );

	do
	{
		auto var = VariableFactory::create( xvar, xdoms, agents );
		variables.push_back(var);
      
		//std::cout << var->to_string() << std::endl;

		xvar = xvar->next_sibling();
	} while ( xvar );
  
	ASSERT( nb_variables == variables.size(), "Number of variables read " 
		<< variables.size() << " differs from the number of variables declared.");
}

void Problem::parseXMLConstraints(xml_node<>* root)
{
	// Parse and create constraints
	xml_node<>* xrels = root->first_node("relations");
	xml_node<>* xcons = root->first_node("constraints");
	int size = atoi( xcons->first_attribute("nbConstraints")->value() );
	if (size > 0)
	{
		xml_node<>* xcon  = xcons->first_node("constraint");
		do {
			auto con =
				ConstraintFactory::create( xcon, xrels, agents, variables );
			constraints.push_back(con);
      
			// std::cout << con->to_string() << std::endl;
      
			xcon = xcon->next_sibling();
		} while ( xcon );
	}

	ASSERT( size == constraints.size(), "Number of constraints read " 
		<< constraints.size() << " differs from the number of items declared.");
}


//////////////////////////////////////////////
void Problem::importWCSP(std::string fileName, InputParams::agent_t agtType) {
	int size = 0;
	std::string line;
	std::string trash;
	std::ifstream in( fileName.c_str() );
  
	ASSERT(in.is_open(), "Error: cannot open the input file.");
  
	// Read Preamble
	std:getline(in, line);
	int nVars,  maxDomSize,  nCons; long int UB;
	{ 
		std::stringstream data(line);
		data >> trash >> nVars >> maxDomSize >> nCons >> UB;
		// std::cout <<"UB=" << UB << std::endl;
	}

	// Read Domains (create agents and variables)
	std::getline(in, line);
	{
		std::stringstream data(line);
		int domSize;
		for (int id=0; id<nVars; id++) {
			data >> domSize;
			auto agt = AgentFactory::create("a_" + std::to_string(id), agtType, 
			InputParams::getAgentParam());
			auto var = VariableFactory::create("x_" + std::to_string(id),
			agt, 0, domSize-1);

			agents.push_back(agt);
			variables.push_back(var);
		}
	}

	// Read constraints
	while (std::getline(in, line)) {
		std::stringstream data(line);
		int cArity; std::vector<int> scopeVarID; util_t defCost; size_t nTuples;
		data >> cArity;
		scopeVarID.resize(cArity);
		for (int i=0; i<cArity; i++)
			data >> scopeVarID[i];
		data >> defCost >> nTuples;
		std::string conData;

		for (int i=0; i<nTuples; i++){
			std::getline(in, line);
			conData += line + "\n";
		}
		// if (defCost == UB) defCost = Constants::unsat;
		auto con = ConstraintFactory::create(nTuples, defCost, UB, conData,
		scopeVarID, agents, variables);

		constraints.push_back(con);
	}

}


//////////////////////////////////////////////
void Problem::importUAI(std::string fileName, InputParams::agent_t agtType) {
	int size = 0;
	std::string line;
	std::string trash;
	std::ifstream in( fileName.c_str() );
  
	ASSERT(in.is_open(), "Error: cannot open the input file.");
	bool Bayes = false;
	bool Markov = false;
	
	// Read Preamble (search for MARKOV)
	std:getline(in, line);
	Markov = (line.find("MARKOV") != std::string::npos);
	Bayes = (line.find("BAYES") != std::string::npos);
	ASSERT(Bayes || Markov, "Missing preamble UAI format");
    
	// Read the number of variables
	int nVars;
	std::getline(in, line);
	{
		std::stringstream data(line);
		data >> nVars;
	}
    
	// Read Domains (create agents and variables)
	std::vector<int> domSizes(nVars);
	std::getline(in, line);
	{
		std::stringstream data(line);
		for (int vId = 0; vId < nVars; vId++) {
			data >> domSizes[vId];
			auto agt = AgentFactory::create("a_" + std::to_string(vId), agtType, 
			InputParams::getAgentParam());
			auto var = VariableFactory::create("x_" + std::to_string(vId),
			agt, 0, domSizes[vId]-1);
        
			agents.push_back(agt);
			variables.push_back(var);
		}
	}
    
	// Read the number of cliques
	int nCliques;
	std::getline(in, line);
	{
		std::stringstream data(line);
		data >> nCliques;
	}
    
	// Read cliques
	std::vector<std::vector<int>> cliques(nCliques);
	for (int cId = 0; cId < nCliques; cId++) {
		std::getline(in, line);
		std::stringstream data(line);
		int cliqueSize, vId;
		data >> cliqueSize;
		for (int i = 0; i < cliqueSize; i++) {
			data >> vId;
			cliques[cId].push_back(vId);
		}
	}
    
	
	// Read Function Tables
	for (int cId = 0; cId < nCliques; cId++) {
		size_t nEntries, nRows;
		auto cliqueVars = cliques[cId]; // contains the Vars ID in the clique
		int  arity = cliques.size();
		double prob;

		std::getline(in, line);
		if (line.empty())  // skip empty line?
			std::getline(in, line);

		// Read number of entries = product of the domain sizes of the varialbes in the scope
		std::stringstream data(line);
		data >> nEntries;
      	std::vector<util_t> entries;
		
		while (entries.size() < nEntries) {
			std::getline(in, line);
			for (auto d : strutils::vstod(strutils::split(line))) {
				entries.push_back(d);
			}
		}
      
		//auto con = ConstraintFactory::create(nEntries, line, cliqueVars, agents, variables);
		auto con = ConstraintFactory::create(entries, cliqueVars, agents, variables);
		constraints.push_back(con);		
	}
    
}




////////////////////////////////////////////
// PSEUDO-TREE ORDERING
///////////////////////////////////////////
bool Problem::order_des(int LHS, int RHS) { 
	return LHS > RHS; 
}

bool Problem::order_asc(int LHS, int RHS) { 
	return LHS < RHS; 
}

bool Problem::lex_asc(int LHS, int RHS) {
	std::string strL = std::to_string(LHS);
	std::string strR = std::to_string(RHS);
	return //std::to_string(LHS).compare(std::to_string(RHS));
	std::lexicographical_compare(strL.begin(), strL.end(), strR.begin(), strR.end());
}

bool Problem::lex_des(int LHS, int RHS) {
	std::string strL = std::to_string(LHS);
	std::string strR = std::to_string(RHS);
	return //std::to_string(RHS).compare(std::to_string(LHS));
	std::lexicographical_compare(strR.begin(), strR.end(), strL.begin(), strL.end());
}

bool Problem::order_neig_asc(int LHS, int RHS) {
	return getAgent(LHS)->getNbNeighbors() < getAgent(RHS)->getNbNeighbors();
}

bool Problem::order_neig_des(int LHS, int RHS)  {
	return getAgent(LHS)->getNbNeighbors() > getAgent( RHS )->getNbNeighbors();
}


void Problem::makePseudoTreeOrder() {
	if (!Preferences::silent) {
		std::cout << "Construct (good) PseudoTree..."; std::flush(std::cout);
		//std::cout << "Searching for a good pseudo-tree" << std::endl;
	}

	// Check if pseudoTree exists
	if (existsSavedPseudoTree()) {
		loadPseudoTree();
		if (!Preferences::silent) {
			std::cout << "\tok " << "(w*=" << getInducedWidth() << ")\n";
		}
		return;
	}
    

	std::vector<std::vector<int>> forest = Problem::getForest();

	int level = 0;
	// for (auto& tree : forest) {    
	//   std::cout << "Forest: :";
	//   for (auto i:tree) std::cout << i << " ";
	//   std::cout << "\n";
	// }
  
	std::vector<int> vec_r;
	std::vector<int> vec_h;
  
	for (auto& tree : forest) { 
		int best_w = -1;
		int best_h = -1;
		int best_a = -1;
       
		for (auto agtId : tree) {
			for (int h=0; h<=5; h++) {
				makePseudoTreeOrder(tree, agtId, h, level);
				int w = getInducedWidth(tree);
				// std::cout << " w = " << w << std::endl;
        
				if (best_w == -1 || w < best_w) {
					best_w = w;
					best_h = h;
					best_a = agtId;
				}
			}//-heur
		}//-tree
		makePseudoTreeOrder(tree, best_a, best_h, level);
		vec_r.push_back(best_a);
		vec_h.push_back(best_h);

		level += tree.size();
	}//-forest

	// Save PseudoTree 
	savePseudoTree(vec_r, vec_h);

	if (!Preferences::silent) {
		std::cout << "\tok " << "(w*=" << getInducedWidth() << ")\n";
	}
}


void Problem::makePseudoTreeOrder(int root, int heur) {

	std::vector<std::vector<int>> forest = Problem::getForest();
	std::vector<int> vec_r;
	std::vector<int> vec_h;


	int level = 0;
	for (auto& tree : forest) {
		int best_w = -1;
		int best_h = heur;

		int r = tree[0];
		if (utils::find(root, tree)) {
			r = root;
		}

		if (best_h == -1) { 
			for (int h=0; h<=5; h++) {
				makePseudoTreeOrder(tree, r, h, -1);
				int w = getInducedWidth(tree);    
				if (best_w == -1 || w < best_w) {
					best_w = w;
					best_h = h;
				}
			}//-heur
		}
		makePseudoTreeOrder(tree, r, best_h, level);
		vec_r.push_back(r);
		vec_h.push_back(best_h);

		level+= tree.size();

	}//-tree

	savePseudoTree(vec_r, vec_h);
  
}


// level = starting level of this group of agents = prev group size + 1
void Problem::makePseudoTreeOrder
(std::vector<int> agentsId, int root, int heur, int level) {
	Preferences::ptRoot = root;
	Preferences::ptHeuristic = heur;
  
	std::map<int, bool> discovered;
	for (auto agtId : agentsId) {
		discovered[ agtId ] = false; // set discoverable
		getAgent(agtId)->clearOrder();
	}
  
	std::stack<int> S;
	S.push( root );
	getAgent(root)->setParent(nullptr);
  
	// DFS exploration
	while (!S.empty()) {
		int ai = S.top(); S.pop();
    
		if (!discovered[ ai ]) {   
			// Get neighbors of ai and order them 
			std::vector<int> N;
			for (auto a : getAgent(ai)->getNeighbors())
				N.push_back(a->getID());
      
			if (heur == 0)
				std::sort(N.begin(), N.end(), order_asc); // default
			else if (heur == 1)
				std::sort(N.begin(), N.end(), order_des);  //
			else if (heur == 2)
				std::sort(N.begin(), N.end(), order_neig_asc);  // (frodo default?)
			else if (heur == 3)
				std::sort(N.begin(), N.end(), order_neig_des);  //
			else if (heur == 4)
				std::sort(N.begin(), N.end(), lex_asc);  //
			else if (heur == 5)
				std::sort(N.begin(), N.end(), lex_des);  //
            
			for (int ci : N ) {
				if (!getAgent(ai)->isRoot() && 
					ci == getAgent(ai)->getParent()->getID() ) 
						continue;
        
				S.push( ci );
        
				// Children of ai
				if (!discovered[ ci ]) {
					getAgent(ai)->addChild( getAgent(ci) );  // ci is child of ai
					getAgent(ci)->setParent( getAgent(ai) ); // ai is parent of ci
				}
				else {
					// Set back-edges
					getAgent(ai)->addPseudoParent( getAgent(ci) );
					getAgent(ci)->addPseudoChild( getAgent(ai) );
					getAgent(ci)->removeChild( getAgent(ai) );
				}
			}//-neighbors
      
			discovered[ ai ] = true;
      
		}//-not discovered
	}// while Stack is not empty
  
	// Check Graph is connected
	for (auto& kv : discovered) {
		ASSERT(kv.second, "Error: The constraint graph is not connected");
	}
  
	// Set separator set.
	for (auto agtId : agentsId)
		getAgent(agtId)->getSeparator(); // it also builds one
  
	// Set Agents priorities - start from the root and recur:
	if (level >= 0)
		setAgentsPriorities(root, level);
}


//
// Generate Forest
//
std::vector<std::vector<int>> Problem::getForest() {
  
	std::vector<std::vector<int>> forest;
	std::map<int, bool> discovered;

	for (auto agt : Problem::getAgents()) {
		discovered[ agt->getID() ] = false; // set discoverable
	}

	int root = Preferences::default_ptRoot;
	while (utils::findFirstValue(discovered, false) != discovered.end()) { 

		// If this root has already been processsed, choose another root
		if (discovered[root]) {
			root = utils::findFirstValue(discovered, false)->first;
		}

		std::vector<int> tree;
		std::stack<int> S;
		S.push( root );
		// DFS exploration
		while (!S.empty()) {
			int ai = S.top(); S.pop();
      
			if (!discovered[ ai ]) {   
				for (auto& ci : getAgent(ai)->getNeighbors() ) {
					S.push( ci->getID() );        
				}
				discovered[ ai ] = true;
				tree.push_back(ai);
			}
		}// end DFS
		forest.push_back(tree);
	}

	return forest;
}


void Problem::setAgentsPriorities(int root, int root_p) {
	std::queue<Agent::ptr> Q;
	auto rootAgt = getAgent(root);
	rootAgt->setPriority(root_p);
	Q.push(rootAgt); 

	while (!Q.empty()) {
		auto agt = Q.front(); Q.pop();
		int p = agt->getPriority();
		for (auto chAgt : agt->getChildren()) {
			chAgt->setPriority(p+1);
			Q.push(chAgt);
		}
	} //-
}

int Problem::getInducedWidth(std::vector<int> tree) {
	int w_star = -1;
	for (int aId : tree) {
		int w = getAgent(aId)->getSeparator().size();
		if (w > w_star) w_star = w;
	}
	return w_star;
}


int Problem::getInducedWidth() {
	int w_star = -1;
	for (auto a : Problem::agents) {
		int w = a->getSeparator().size();
		if (w > w_star) w_star = w;
	}
	return w_star;
}



bool Problem::existsSavedPseudoTree() {
	std::string file = InputParams::getFileName();
	file = file.substr(0, file.find_last_of("."));
	file += ".ptf";// pseudo-tree format
	std::ifstream f(file.c_str());
	return f.good();
}

void Problem::loadPseudoTree() {
	std::string file = InputParams::getFileName();
	file = file.substr(0, file.find_last_of("."));
	file += ".ptf";// pseudo-tree format
	std::string sep = " ";
	std::ifstream ifs;
	ifs.open(file.c_str(), std::ifstream::in);
  
	std::string line;
	//TODO: Reinsert this line
	// getline(ifs, line); // skip first line (root and heuristics)

	while (getline(ifs, line)) {
		std::stringstream data(line);
		int agtId, priority, size;
		// Agent and priority
		data >> agtId >> priority >> size;
		auto agt = Problem::getAgent(agtId);
		agt->setPriority(priority);    
		// Parent
		if (size == 1) {
			data >> agtId;
			agt->setParent(Problem::getAgent(agtId));
		} else {
			agt->setParent(nullptr);
		}
		// PseudoParents
		data >> size;
		for (int i=0; i<size; i++){
			data >> agtId;
			agt->addPseudoParent(Problem::getAgent(agtId));
		}
		// Children
		data >> size;
		for (int i=0; i<size; i++){
			data >> agtId;
			agt->addChild(Problem::getAgent(agtId));
		}
		// PseudoChildren
		data >> size;
		for (int i=0; i<size; i++){
			data >> agtId;
			agt->addPseudoChild(Problem::getAgent(agtId));
		}
	}

	ifs.close();
  
	// Set separator set.
	for (auto agt : Problem::agents)
		agt->getSeparator(); // it  builds one
}


void Problem::savePseudoTree(std::vector<int> vec_r, std::vector<int> vec_h) {
	std::string file = InputParams::getFileName();
	file = file.substr(0, file.find_last_of("."));
	file += ".ptf";// pseudo-tree format
	std::string sep = " ";
	std::ofstream ofs;
	ofs.open(file.c_str(), std::ofstream::out);

	// Root node and Heuristics
	ofs << vec_r.size() << sep;  // size of the forst
	for (int i=0; i<vec_r.size(); i++)
		ofs << vec_r[i] << sep << vec_h[i] << sep;
	ofs << std::endl;

	for (auto agt: Problem::agents){
		// Agent and Priority
		ofs << agt->getID() << sep << agt->getPriority() << sep;
		// Parent
		if (agt->isRoot()) ofs << "0" << sep;
		else               ofs << "1" << sep << agt->getParent()->getID() << sep;
		// PseudoParents
		ofs << agt->getPseudoParents().size() << sep;
		for (auto ai: agt->getPseudoParents())
			ofs << ai->getID() << sep;

		// Children
		ofs << agt->getChildren().size() << sep;
		for (auto ai: agt->getChildren())
			ofs << ai->getID() << sep;

		// PseudoChildren
		ofs << agt->getPseudoChildren().size() << sep;
		for (auto ai: agt->getPseudoChildren())
			ofs << ai->getID() << sep;          
		ofs << std::endl;
	}
	ofs.close();
}
