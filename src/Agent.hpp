//
// Created by Ferdinando Fioretto on 10/31/15.
//

#ifndef CUDA_DBE_AGENT_H
#define CUDA_DBE_AGENT_H

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <cmath>

#include "utils.hpp"
#include "Variable.hpp"
#include "Constraint.hpp"

using namespace misc_utils;

class Agent : public std::enable_shared_from_this<Agent> {
public:
  typedef std::shared_ptr<Agent> ptr;

  static bool orderLt (const ptr& lhs, const ptr& rhs) {
    if (lhs->getPriority() < rhs->getPriority()) return true;
    return lhs->getPriority() == rhs->getPriority() && 
      lhs->getID() < rhs->getID();
  }

  static bool orderGt (const ptr& lhs, const ptr& rhs) {
    if (lhs->getPriority() > rhs->getPriority()) return true;
    return lhs->getPriority() == rhs->getPriority() && 
      lhs->getID() > rhs->getID();
  }
  
  Agent() : ID(0), util(0), priority(0){ }

  Agent(int _ID, std::string _name)
    : ID(_ID), name(_name), util(0), priority(0)
    { }
  
  bool operator==(const Agent& rhs) const {
    return ID == rhs.getID();
  }

  
  bool operator==(const std::shared_ptr<Agent>& rhs ) const {
    return ID == rhs->getID();
  }
  
  virtual void initialize() = 0;

  virtual void utilPhaseAggr()  = 0;

  virtual void utilPhaseProj()  = 0;

  virtual void valuePhase() = 0;

  virtual util_t getUtil() {
    return util;
  }

  virtual void setUtil(util_t _util) {
#ifdef MAP
//	  std::cout << "setting util: " << log(_util) << "\n";
	  util = log(_util);
#endif
#ifdef MPE
//	  std::cout << "setting util: " << log(_util) << "\n";
    util = log(_util);
#endif
#ifdef WCSPmax
	util = _util;
#endif
#ifdef WCSPmin
	util = _util;
#endif
  }

  std::shared_ptr<Agent> getPtr() {
    return shared_from_this();
  }

  void addVariable(Variable::ptr& var) {
    auto it = std::find(variables.begin(), variables.end(), var);
    if (it == variables.end())
      variables.push_back(var);
  }

  void addConstraint(Constraint::ptr& con) {
    auto it = std::find(constraints.begin(), constraints.end(), con);
    if (it == constraints.end()) {
      constraints.push_back(con);
      for (auto& v : con->getScope()) {
	addNeighbor(v->getAgt());
      }
    }
  }

  void addNeighbor(Agent::ptr& agt) {
    auto it = std::find(neighbors.begin(), neighbors.end(), agt);
    if (it == neighbors.end() && agt->getID() != ID)
      neighbors.push_back(agt);
  }

  int getNbNeighbors() const {
    return neighbors.size();
  }
  
  Agent::ptr getNeighbor(int i) {
    return neighbors[i];
  }
  
  int getID() const {
    return ID;
  }
  
  const std::string &getName() const {
    return name;
  }
  
  std::vector<ptr> &getNeighbors() {
    return neighbors;
  }
  
  std::vector<Variable::ptr> &getVariables() {
    return variables;
  }
  
  Variable::ptr& getVariable(int i=0) {
    return variables[i];
  }
  
  std::vector<Constraint::ptr> &getConstraints() {
    return constraints;
  }
  
  Constraint::ptr& getConstraint(int i) {
    return constraints[i];
  }

  bool isRoot() {
    return parent == nullptr;
  }

  bool isLeaf() {
    return children.empty();
  }
  
  void setParent(Agent::ptr agt) {
    parent = agt;
  }

  void addChild(Agent::ptr agt) {
    auto it = std::find(children.begin(), children.end(), agt);
    if (it == children.end() && agt->getID() != ID)
      children.push_back(agt);
  }

  void addPseudoParent(Agent::ptr agt) {
    auto it = std::find(pseudoParents.begin(), pseudoParents.end(), agt);
    if (it == pseudoParents.end() && agt->getID() != ID)
      pseudoParents.push_back(agt);
  }

  void addPseudoChild(Agent::ptr agt) {
    auto it = std::find(pseudoChildren.begin(), pseudoChildren.end(), agt);
    if (it == pseudoChildren.end() && agt->getID() != ID)
      pseudoChildren.push_back(agt);
  }
  
  void removeChild(Agent::ptr agt) {
    auto it = children.begin();
    while (it != children.end()) {
      if ((*it)->getID() == agt->getID()) {
	children.erase(it);
	return;
      }
      it++;
    }
  }

  std::vector<Agent::ptr> getSeparator() {
    if (!separator.empty() || isRoot())
      return separator; 
    
    separator = misc_utils::utils::concat(parent, pseudoParents);
    if (isLeaf()) return separator;

    for (auto c : children) {
      misc_utils::utils::merge_emplace(separator, c->getSeparator());
    }
    misc_utils::utils::exclude_emplace(children, separator);
    misc_utils::utils::exclude_emplace(getPtr(), separator);
    
    return separator;
  }


  std::vector<Constraint::ptr> getAncestorsConstraints() {
    std::vector<Constraint::ptr> ret;
    if (isRoot()) return ret;

    std::vector<int> ancestorsID;
    std::vector<int> successorsID;

    ancestorsID.push_back(parent->getID());
    for (auto agt : pseudoParents) ancestorsID.push_back(agt->getID());

    for (auto agt : children) successorsID.push_back(agt->getID());
    for (auto agt : pseudoChildren) successorsID.push_back(agt->getID());

    for (auto con : constraints) {
      auto conScope = con->getScopeAgentID();
      // TODO: This op is not efficient - i can speed it up...
      if (!utils::intersect(conScope, ancestorsID).empty() && 
          utils::intersect(conScope, successorsID).empty())
	ret.push_back(con);
    }
    return ret;
  }

  std::vector<Constraint::ptr> getUnaryConstraints() {
    std::vector<Constraint::ptr> ret;
    for (auto con : constraints) {
      if (con->getArity() == 1)
	ret.push_back(con);
    }
    return ret;
    //ancestorsID.push_back(getID());
  }

  Agent::ptr getParent() {
    return parent;
  }

  std::vector<Agent::ptr>& getChildren() {
    return children;
  }

  std::vector<Agent::ptr>& getPseudoParents() {
    return pseudoParents;
  }

  std::vector<Agent::ptr>& getPseudoChildren() {
    return pseudoChildren;
  }

  void setPriority(int h) {
    priority = h ;
  }
  
  int getPriority() const {
    return priority;
  }

  void clearOrder() {
    priority = 0;
    parent = nullptr;
    children.clear();
    pseudoChildren.clear();
    pseudoParents.clear();
    separator.clear();
  }

  
  std::string to_string() {
    std::string ret = "Agt: " + name + "(ID=" + std::to_string(ID) + ")\n";
    ret += " Var:";
    for (auto var : variables)
      ret += "\t" + var->to_string() + "\n";
    
    ret += " Cons:";
    for (auto con : constraints)
      ret += "\t" + con->to_string() + "\n";
    
    ret += " Neighbors=\t{"; 
    for (auto n : neighbors)
      ret += std::to_string(n->getID()) + ", ";
    ret += "}";


    ret += "\n";
    ret += "P="; 
    if (!isRoot()) ret+= std::to_string(parent->getID());
    else ret += "-";
    ret += "\tPP={";
    for (auto agt: pseudoParents) ret += std::to_string(agt->getID()) + ",";
    ret += "}\tC={";
    for (auto agt: children) ret += std::to_string(agt->getID()) + ",";
    ret += "}\tPC={";
    for (auto agt: pseudoChildren) ret += std::to_string(agt->getID()) + ",";
    ret += "}\tSep={";
    for (auto agt: separator) ret += std::to_string(agt->getID()) + ",";
    ret += "}\n";
    
    ret += "Priority = " + std::to_string(priority) + "\n";
    return ret;
  }

  
protected:
  int ID;
  std::string name;
  int priority; // the hight of this agent in the pseudo-tree where root = 1 (lower)
  // and leaves have the highest priority

  util_t util;
  
  std::vector<std::shared_ptr<Agent>> neighbors;
  std::vector<std::shared_ptr<Variable>> variables;
  std::vector<std::shared_ptr<Constraint>> constraints;

  // PseudoTree 
  Agent::ptr parent;
  std::vector<Agent::ptr> children;
  std::vector<Agent::ptr> pseudoParents;
  std::vector<Agent::ptr> pseudoChildren;
  std::vector<Agent::ptr> separator;
};

#endif //D_AGC_DR_BUSAGENT_H
