//
// Created by Ferdinando Fioretto on 11/6/15.
//

#ifndef CUDA_DBE_PERMUTATION_H
#define CUDA_DBE_PERMUTATION_H

#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>

#include "string_utils.hpp"

using namespace misc_utils;

namespace combinatorics {

	template <class T>
	class Permutation {
	public:
		typedef std::shared_ptr<Permutation> ptr;
		typedef std::vector<std::vector<T>> Permutations;
    
		// Reserve space here rather than pushnig back (duplicates)
		Permutation (std::vector<T> upperBounds) {
			k = upperBounds.size();
			values.resize(k);
			tmpValues.resize(k, 0);
			for (int i=0; i<k; i++){
				for (int v=0; v <= upperBounds[i]; v++) {
					values[i].push_back(v);
				}
			}
			level = 0;
			generate();
		}

		Permutation(std::vector<std::vector<T>> _values)
		: values(_values){
			k = values.size();
			tmpValues.resize(k, 0);
			level = 0;
			generate();
		}
    
		std::vector<std::vector<T>>& getPermutations() {
			return permutations;
		}
    
		size_t size() const {
			return permutations.size();
		}
    
		inline
		void generate() {
			if (level == k) {
				permutations.push_back(tmpValues);
				return;
			}

			for (int i = 0; i < values[level].size(); i++) {
				tmpValues[level] = values[level][i];
				level++;
				generate();
				level--;
			}
		}
    
		std::string to_string() const {
			std::string res;
			for (auto& v : permutations) {
				res += strutils::to_string(v) + "\n";
			}
			return res;
		}
    
	private:
		std::vector<std::vector<T>> values;

		int k;
		std::vector<std::vector<T>> permutations;
    
		int level;
		std::vector<T> tmpValues;
		T limit;
	};
  
};


#endif //D_AGC_DR_PERMUTATION_H
