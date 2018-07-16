#ifndef DARCEL_CONSTRAINT_RESULT_HPP
#define DARCEL_CONSTRAINT_RESULT_HPP
#include <memory>
#include <unordered_map>
#include "darcel/data_types/data_types.hpp"
#include "darcel/semantics/semantics.hpp"
#include "darcel/type_checks/type_checks.hpp"

namespace darcel {

  //! Stores the result of a constraints test.
  struct constraint_result {

    //! Whether the constraints were satisfied.
    bool m_is_satisfied = false;

    //! The variable conversions needed to satisfy the constraints.
    std::unordered_map<std::shared_ptr<variable>, std::shared_ptr<data_type>>
      m_conversions;
  };
}

#endif
