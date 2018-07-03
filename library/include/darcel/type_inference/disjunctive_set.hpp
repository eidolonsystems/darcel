#ifndef DARCEL_DISJUNCTIVE_SET_HPP
#define DARCEL_DISJUNCTIVE_SET_HPP
#include "darcel/type_inference/conjunctive_set.hpp"
#include "darcel/type_inference/type_inference.hpp"

namespace darcel {

  //! Stores a set of disjunctive requirements where only one of the
  //! requirements must be satisfied.
  class disjunctive_set {
    public:
      void add(conjunctive_set s);
  };
}

#endif
