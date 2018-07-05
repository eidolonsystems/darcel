#ifndef DARCEL_CONSTRAINTS_HPP
#define DARCEL_CONSTRAINTS_HPP
#include "darcel/type_checker/disjunctive_set.hpp"
#include "darcel/type_checker/type_checker.hpp"

namespace darcel {

  //! Stores a set of data type/expression requirements where all requirements
  //! must be satisfied.
  class constraints {
    public:

      void add(disjunctive_set s);
  };
}

#endif
