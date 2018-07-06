#ifndef DARCEL_TYPE_CHECKER_HPP
#define DARCEL_TYPE_CHECKER_HPP
#include "darcel/syntax/syntax_node.hpp"
#include "darcel/type_checks/type_checks.hpp"

namespace darcel {
  class type_checker {
    public:

      std::shared_ptr<data_type> get_type(const expression& e) const;

      std::shared_ptr<data_type> get_type(const variable& v) const;

      void feed(const syntax_node& node);
  };
}

#endif
