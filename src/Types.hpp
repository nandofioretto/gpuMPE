//
// Created by Ferdinando Fioretto on 11/7/15.
//

#ifndef TYPES_H
#define TYPES_H

#include <limits>
#include "Preferences.hpp"

#ifdef MAP
// UAI  Speed 
typedef int value_t;
typedef double util_t;
// UAI  Precision
// typedef int    value_t;
// typedef double  util_t;
#endif
#ifdef MPE	// Most Probable Explanation
typedef unsigned short value_t;
typedef float util_t;
#endif
#ifdef WCSPmax	// WCSP maximize
// WCSPs Speed: 
typedef unsigned short value_t;
typedef unsigned short util_t;
// WCSPs Precision
// typedef int value_t;
// typedef int util_t;
#endif
#ifdef WCSPmin	// WCSP minimize
// WCSPs Speed: 
typedef unsigned short value_t;
typedef unsigned short util_t;  
#endif

	
	
// Constants 
class Constants {
public:
  static constexpr value_t NaN   = std::numeric_limits<value_t>::min()+1;
  static constexpr util_t inf    = 
#ifdef MINIMIZE
    std::numeric_limits<util_t>::max();
#else
    std::numeric_limits<util_t>::min();
#endif

  static constexpr util_t unsat  = inf;
  // move this to u_math:: namespace
  static constexpr bool isFinite(util_t c) { return c != inf && c != -inf; } 
  static constexpr bool isSat(util_t c) { return c != unsat; }

  static constexpr util_t worstvalue = inf;
  // static constexpr util_t bestvalue  = inf;

  static util_t aggregate(util_t a, util_t b) {
    return (a != unsat && b != unsat)? a + b : unsat; 
  }

private:
  Constants(){ }
};

#endif //TYPES_H
