#ifndef CUDA_DBE_TABLE_CONSTRAINT_HPP
#define CUDA_DBE_TABLE_CONSTRAINT_HPP

#include <map>
#include <vector>
#include <string>

#include "Types.hpp"
#include "Variable.hpp"
#include "Constraint.hpp"


// It defines the Abstract class for Extensional Soft Constraints.
// TODO: The aim is to implement it as a TABLE CONSTRAINT
class TableConstraint : public Constraint 
{
public:
  typedef std::shared_ptr<TableConstraint> ptr;

  // It sets the constraint type, scope, number of tuples contained in 
  // the explicit relation and default util.
  TableConstraint
  (std::vector<Variable::ptr>& _scope, util_t _defUtil)
    : defaultUtil(_defUtil), LB(Constants::inf), UB(Constants::inf)
  {
   Constraint::scope = _scope;
   for(auto& v : Constraint::scope) {
     scopeAgtID.push_back(v->getAgtID());
   }
  }

  ~TableConstraint() { };
  
  void setUtil(std::vector<value_t>& K, util_t _util) {
    values[ K ] = _util;
  }

  // It returns the util associated to the value assignemnt passed as a 
  // parameter.
  util_t getUtil(std::vector<value_t>& K) {
    auto it = values.find(K);
    return (it != values.end()) ? it->second : defaultUtil;
  }

  util_t getDefaultUtil() const { 
    return defaultUtil;
  }

  size_t getSizeBytes() const {
    return values.size() * (getArity()+1) * sizeof( size_t );
  }

  // It returns a Summary Description.
  virtual std::string to_string() {
    std::string result = Constraint::name + " scope={";
    for( int i=0; i<getArity(); i++)
      result += std::to_string(scope[ i ]->getAgtID()) + ", ";
    result += "}";

    result+="\n";
    for (auto& kv: values) {
      result += "<";
      for (int i : kv.first)
    	  result += std::to_string(i) + " ";
      result += "> : ";
    	  result += std::to_string(kv.second) + " ";
      result += "\n";
    }

    return result;
  }
  
 protected:
  std::map<std::vector<value_t>, util_t> values;

  // Default util, that is the util associated to any value combination that is
  // not explicitally listed in the relation.
  util_t defaultUtil;

  // The best and worst finite utils of this constraint which is used as bounds
  // in some searches.
  util_t LB;
  util_t UB;

};

#endif // CUDA_DBE_TABLE_CONSTRAINT_HPP
