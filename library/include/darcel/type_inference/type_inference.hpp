#ifndef DARCEL_TYPE_INFERENCE_HPP
#define DARCEL_TYPE_INFERENCE_HPP
#include <memory>
#include <unordered_map>
#include "darcel/data_types/data_types.hpp"
#include "darcel/syntax/syntax.hpp"

namespace darcel {
  class conjunctive_set;
  class constraints;
  class disjunctive_set;
  using type_map = std::unordered_map<
    const expression*, std::shared_ptr<data_type>>;
}

#endif
