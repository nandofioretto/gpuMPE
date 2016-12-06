#ifndef CUDA_DBE_GPUTABLE_CUH
#define CUDA_DBE_GPUTABLE_CUH

#include <memory>
#include <vector>

#include "Agent.hpp"
#include "Constraint.hpp"
#include "Variable.hpp"
#include "Types.hpp"

using namespace misc_utils;


class GpuTable {
public:
  typedef std::shared_ptr<GpuTable> ptr;

  GpuTable(std::vector<Variable::ptr> _vars) 
    : scope(_vars), values(nullptr), utils(nullptr), nbEntries(0)
  { }

  ~GpuTable();

  // Create the CPU Table Associated to the Constraint con
  GpuTable(Constraint::ptr con);

  // Creates an Empty CPU Table Associaed to the set of variables given
  GpuTable(std::vector<Agent::ptr> agts);

  size_t getSize() const {
    return nbEntries;;
  }
  
  std::vector<Variable::ptr>& getScope() {
    return scope;
  }

  util_t* getUtils() {
    return utils;
  }

  void update(std::vector<util_t> hostUtils, std::vector<Variable::ptr> vars);
 
  void update(std::vector<Variable::ptr> vars, size_t newTabSize) {
    nbEntries = newTabSize;
    scope = vars;
  }

  size_t getSizeBytes() const {
    return nbEntries * scope.size() * sizeof( value_t );
  }

  std::string to_string();  
  
private:
  std::vector<Variable::ptr> scope;
  // std::vector<std::vector<value_t>> values;
  //std::vector<util_t> utils;
  
  size_t nbEntries;
  value_t* values; // Device Pointer to Values (Not needed in the curr naive version)
  util_t*  utils;  // Device pointer to Utils
};

#endif // CUDA_DBE_CPUTABLE_HPP
