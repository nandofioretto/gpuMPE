#include <iostream>
#include <utility>

#include <utils.hpp>

#include "CpuProjector.hpp"
#include "Preferences.hpp"
#include "Assert.hpp"
#include "Types.hpp"
#include "CpuTable.hpp"

CpuTable::ptr 
CpuProjector::project(const CpuTable::ptr& in, const Variable::ptr& var) {
  ASSERT(in->getScope().back()->getAgtID() == var->getAgtID(),
	 "Projection done over bad ordered variables");
  
  auto scope = in->getScope();
  std::vector<Variable::ptr> newScope(scope.begin(), scope.end()-1);
  auto projTable = std::make_shared<CpuTable>(newScope);
  size_t varDomSize = var->getDomSize();
  size_t newTabSize = in->getSize() / varDomSize;
  
  for (size_t r = 0; r < newTabSize; r++) { 
    size_t bestIdx = r*varDomSize;
    util_t bestUtil = 0;

    for (int d = 0; d < varDomSize; d++) {
      auto util = in->getUtil(r*varDomSize + d);
#ifndef MPE
      if (bestUtil RelOP util)
        bestIdx = r*varDomSize + d;
#endif
      bestUtil = ProjOP(bestUtil, util);
    }
    auto& val = in->getValue(bestIdx);      
    projTable->pushRow(std::vector<value_t>(val.begin(), val.end()-1), bestUtil);
  }
  
  return projTable;
}


std::pair<value_t, util_t>
CpuProjector::project(const CpuTable::ptr& in) {
  value_t bestVal = in->getValue(0)[0];  // TODO: check this operation
  
  util_t bestUtil = 0;

  // Choose best Value
  for (int r=0; r<in->getSize(); r++) {
    auto util = in->getUtil(r);
#ifndef MPE
    if (bestUtil RelOP util)
      bestVal = r;
#endif
    bestUtil = ProjOP(bestUtil, util);
  }
  return std::make_pair<>(bestVal, bestUtil);
}
