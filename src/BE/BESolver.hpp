//
// Created by Ferdinando Fioretto on 11/3/15.
//

#ifndef CUDA_BE_BESOLVER_H
#define CUDA_BE_BESOLVER_H

#include <vector>
#include <map>

#include "Solver.hpp"

using namespace misc_utils;

class BESolver : public Solver {

public:
  typedef std::shared_ptr<BESolver> ptr;
    
  BESolver() {} 

  virtual void solve();

};

#endif //D_AGC_DR_BESOLVER_H
