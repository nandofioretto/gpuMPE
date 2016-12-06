//
// Created by Ferdinando Fioretto on 11/7/15.
//

#ifndef CUDA_DBE_ASSERT_HPP
#define CUDA_DBE_ASSERT_HPP

#include <cassert>
#include <iostream>
#include <string>
#include "Preferences.hpp"

// A macro to disallow the copy constructor and operator= functions.
// It should be used in the private declarations for a class.
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)


// A macro to attach a message to the assert command.
#ifndef NDEBUG
# define ASSERT(condition, message) \
  do {									\
    if (! (condition)) {						\
      std::cerr << "Assertion `" #condition "` failed in " << __FILE__	\
		<< " line " << __LINE__ << ": " << message << std::endl; \
	std::exit(EXIT_FAILURE);					\
    }									\
  } while (false)
#else
# define \
  ASSERT(condition, message) do { } while (false)
#endif

// A macro to print a worning message when a condition is not satisfied
# define WARNING(condition, message) \
  do {									\
    if (! (condition)) {						\
      std::cerr << "Warning: " << message << std::endl;	      		\
    }									\
  } while (false)


namespace Assert {
  static void check(bool condition, std::string msg, std::string err) {
    if (!condition) {
      std::cerr << "Assertion: " << msg << std::endl 
                << "File: " << __FILE__ << " line: " << __LINE__ 
                << std::endl;
      if (Preferences::csvFormat) {
        std::cout << "NA\tNA\t" << err << std::endl;
      }
      std::exit(EXIT_FAILURE);
    }
  }
} 

#endif //  CUDA_DBE_ASSERT_HPP
