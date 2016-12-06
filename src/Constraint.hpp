#ifndef CUDA_DBE__CONSTRAINT_H_
#define CUDA_DBE__CONSTRAINT_H_

#include <vector>
#include <string>

#include "Variable.hpp"
#include "Types.hpp"

// Standard interface class for all constraints.
//
// Defines how to construct a constraint, impose, check satisfiability,
// notSatisfiability, enforce consistency.
class Constraint
{
public:
  typedef std::shared_ptr<Constraint> ptr;

  // It creates a new constraint with empty name and automatically generated ID.
  Constraint() {}

  virtual ~Constraint() {}
  
  // Returns the constraint arity.
  int getArity() const { 
    return scope.size();
  }

  void setID(int _id) {
    Constraint::ID = _id;
  }

  int getID() {
    return ID;
  }

  void setName(std::string _name) {
    Constraint::name = _name;
  }

  std::string getName() {
    return name;
  }

  virtual util_t getUtil(std::vector<value_t>& values) = 0;

  virtual void    setUtil(std::vector<value_t>& values, util_t util) = 0;

  // Returns the constraint scope.
  std::vector<Variable::ptr>& getScope() {
    return scope;
  }

  std::vector<int>& getScopeAgentID() {
    return scopeAgtID;
  }

  // Get the pos-th variable in the constraint scope.
  Variable::ptr getVariableAt(size_t pos) const { 
    return scope[ pos ]; 
  }

  int getAgentIDAt(size_t pos) const {
    return scopeAgtID[pos]; 
  }

  // It returns a Summary Description.
  virtual std::string to_string() = 0;

  // It returns the size of the constraint in bytes.
  virtual size_t getSizeBytes() {return 0; }

protected:
  int ID;
  std::string name;

  // The scope of the constraint
  std::vector<Variable::ptr> scope;
  std::vector<int> scopeAgtID;
};


#endif // ULYSSES_KERNEL__CONSTRAINTS__CONSTRAINT_H_
