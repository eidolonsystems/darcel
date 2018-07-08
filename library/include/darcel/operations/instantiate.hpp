#ifndef DARCEL_OPERATIONS_INSTANTIATE_HPP
#define DARCEL_OPERATIONS_INSTANTIATE_HPP
#include <memory>
#include <unordered_map>
#include "darcel/syntax/syntax_nodes.hpp"
#include "darcel/syntax/syntax_node_visitor.hpp"

namespace darcel {

  //! Instantiates a generic function definition.
  /*!
    \param node The definition to instantiate.
    \param overload The overload to instantiate with.
    \param functions The mapping from variables to their functions.
  */
  inline std::unique_ptr<bind_function_statement> instantiate(
      const bind_function_statement& node, std::shared_ptr<variable> overload,
      std::unordered_map<std::shared_ptr<variable>, std::shared_ptr<function>>&
      functions) {
    return nullptr;
  }
}

#endif
