#ifndef CUDA_DBE_CPUTABLE_HPP
#define CUDA_DBE_CPUTABLE_HPP

#include <memory>
#include <iostream>
#include <vector>
#include <cmath>

#include <string_utils.hpp>
#include "Agent.hpp"
#include "Constraint.hpp"
#include "Variable.hpp"
#include "Types.hpp"

using namespace misc_utils;


class CpuTable {
public:
  typedef std::shared_ptr<CpuTable> ptr;

  CpuTable(std::vector<Variable::ptr> _vars);

  ~CpuTable();

  // Create the CPU Table Associated to the Constraint con
  CpuTable(Constraint::ptr con);

  // Creates an Empty CPU Table Associaed to the set of variables given
  CpuTable(std::vector<Agent::ptr> agts);

  void pushRow(std::vector<value_t> v, util_t u) {
    values.push_back(v);
    utils.push_back(u);
  }

  size_t getSize() const {
    return values.size();
  }
  
  std::vector<Variable::ptr> getScope() const {
    return scope;
  }

  std::vector<std::vector<value_t>>& getValues() {
    return values;
  }

  std::vector<value_t>& getValue(int idx) {
    return values[idx];
  }

  std::vector<util_t>& getUtils() {
    return utils;
  }

  util_t getUtil(int idx) {
    return utils[idx];
  }

  void incrUtil(int idx, util_t u) {
    utils[idx] = utils[idx] JoinOP u;
  }

  size_t getSizeBytes() const {
    return values.size() * scope.size() * sizeof( value_t );
  }

  std::string to_string() const {
    std::string res;
    res += " scope = [";
    for (auto v : scope) res += v->getName() + ",";
    res += "] size :" + std::to_string(getSize()) + "\n";
    //for (int i=0; i<std::min((size_t)10, getSize()); i++) {
    for (int i=0; i<getSize(); i++) {
      res += std::to_string(i) + ": " +
        strutils::to_string(values[i]) + " = " +
    	std::to_string(utils[i]) + "\n";
    }
    return res;
  }
  
  
private:
  std::vector<Variable::ptr> scope;
  std::vector<std::vector<value_t>> values;
  std::vector<util_t> utils;
};

#endif // CUDA_DBE_CPUTABLE_HPP
