#include <iostream>
#include <utils.hpp>
#include <string_utils.hpp>

#include "Types.hpp"
#include "CpuAggregator.hpp"
#include "CpuTable.hpp"

using namespace misc_utils;

//#define QUDRATIC

size_t getIdx(std::vector<size_t> mult, std::vector<size_t> div,
std::vector<int> mod, size_t Tid) {
	size_t size = mult.size();
	size_t inIdx = 0;
	for (int i=0; i<size; i++) {
		inIdx += mult[i]* ((int)(Tid/div[i]) % mod[i]);
	}
	inIdx += Tid % mod[size];
	return inIdx;
}

void CpuAggregator::join(CpuTable::ptr& out, const CpuTable::ptr& in) {
	const int inScopeSize = in->getScope().size();
	const int outScopeSize = out->getScope().size();
  
	// Vector of shift factors
	std::vector<int> InOutIdx(inScopeSize, 0);
	// Populate vector of shift factors - with the associated indexes 
	// in the Out Table
	for (int i_in = 0; i_in < inScopeSize; i_in++) {
		auto in_var = in->getScope()[i_in];
		for (int i_out = 0; i_out < outScopeSize; i_out++) {
			auto out_var = out->getScope()[i_out];
			if(in_var->getAgtID() == out_var->getAgtID()) {
				InOutIdx[i_in] = i_out;
				break;
			}
		}
	}

	std::vector<size_t> mult(inScopeSize-1, 1);
	std::vector<size_t> div(inScopeSize-1, 1);
	std::vector<int> mod(inScopeSize);
  
	for (int k=0; k<inScopeSize-1; k++) {
		// Mult
		for (int j=k+1; j<inScopeSize; j++)
			mult[k] *= in->getScope()[j]->getDomSize();
    
		// Div
		for (int j=InOutIdx[k]+1; j<outScopeSize; j++)
			div[k] *= out->getScope()[j]->getDomSize();

		// Mod
		mod[k] = in->getScope()[k]->getDomSize();
	}
	mod[inScopeSize-1] = in->getScope().back()->getDomSize();
  
	bool eq = false;
	for (int i_out=0; i_out<out->getSize(); i_out++) {
		size_t i_in = getIdx(mult, div, mod, i_out); 
		out->incrUtil(i_out, in->getUtil(i_in));
	}//- outer for
  
}
