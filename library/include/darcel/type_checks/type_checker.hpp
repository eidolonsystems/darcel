#ifndef DARCEL_TYPE_CHECKER_HPP
#define DARCEL_TYPE_CHECKER_HPP
#include "darcel/semantics/variable.hpp"
#include "darcel/syntax/syntax_node_visitor.hpp"
#include "darcel/syntax/syntax_nodes.hpp"
#include "darcel/type_checks/deduce_data_type.hpp"
#include "darcel/type_checks/type_checks.hpp"

namespace darcel {

  //! Performs type checking on syntax nodes.
  class type_checker {
    public:

      //! Returns the data type an expression evaluates to.
      std::shared_ptr<data_type> get_type(const expression& e) const;

      //! Returns a variable's data type.
      std::shared_ptr<data_type> get_type(const variable& v) const;

      //! Type checks a syntax node.
      void check(const syntax_node& node);

    private:
      type_map m_types;
  };

  inline std::shared_ptr<data_type> type_checker::get_type(
      const variable& v) const {
    auto i = m_types.find(&v);
    if(i == m_types.end()) {
      throw syntax_error(syntax_error_code::VARIABLE_NOT_FOUND,
        v.get_location());
    }
    return i->second;
  }

  inline void type_checker::check(const syntax_node& node) {
    struct type_check_visitor final : syntax_node_visitor {
      type_map* m_types;

      void operator ()(type_map& types, const syntax_node& node) {
        m_types = &types;
        node.apply(*this);
      }

      void visit(const bind_function_statement& node) override {
        std::vector<function_data_type::parameter> parameters;
        for(auto& parameter : node.get_parameters()) {
          parameters.emplace_back(parameter.m_variable->get_name(),
            parameter.m_type);
        }
        auto r = deduce_data_type(node.get_expression(), *m_types);
        auto t = std::make_shared<function_data_type>(std::move(parameters),
          std::move(r));
        
//        m_types->insert(std::make_pair(node.get_function().get(), t));
      }

      void visit(const bind_variable_statement& node) override {
        auto t = deduce_data_type(node.get_expression(), *m_types);
        m_types->insert(std::make_pair(node.get_variable().get(), t));
      }
    };
    type_check_visitor()(m_types, node);
  }
}

#endif
