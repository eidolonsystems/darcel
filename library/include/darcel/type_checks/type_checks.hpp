#ifndef DARCEL_TYPE_CHECKS_HPP
#define DARCEL_TYPE_CHECKS_HPP
#include <memory>
#include <unordered_map>
#include "darcel/data_types/data_types.hpp"
#include "darcel/semantics/semantics.hpp"

namespace darcel {
  class conjunctive_set;
  class constraints;
  class disjunctive_set;
  class type_checker;
  using type_map = std::unordered_map<std::shared_ptr<variable>,
    std::shared_ptr<data_type>>;
}

#endif
