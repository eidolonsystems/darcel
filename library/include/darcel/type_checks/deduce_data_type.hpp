#ifndef DARCEL_DEDUCE_DATA_TYPE_HPP
#define DARCEL_DEDUCE_DATA_TYPE_HPP
#include "darcel/semantics/scope.hpp"
#include "darcel/syntax/syntax_node_visitor.hpp"
#include "darcel/syntax/syntax_nodes.hpp"
#include "darcel/type_checks/function_overloads.hpp"
#include "darcel/type_checks/type_checks.hpp"

namespace darcel {

  //! Deduces an expression's data type.
  /*!
    \param e The expression whose data type is to be deduced.
    \param t A map containing previously deduced expression data types.
    \throw syntax_error If the expression has a type error.
  */
  inline std::shared_ptr<data_type> deduce_data_type(const expression& e,
      const type_map& t) {
    struct deduction_visitor final : syntax_node_visitor {
      const type_map* m_type_map;
      std::shared_ptr<data_type> m_result;

      std::shared_ptr<data_type> operator ()(const expression& e,
          const type_map& t) {
        m_type_map = &t;
        e.apply(*this);
        return std::move(m_result);
      }

      void visit(const call_expression& node) override {
        std::vector<function_data_type::parameter> parameters;
        for(auto& parameter : node.get_parameters()) {
          parameters.emplace_back("",
            deduce_data_type(*parameter, *m_type_map));
        }
        auto callable_type =
          [&] {
            if(auto f = dynamic_cast<const function_expression*>(
                &node.get_callable())) {
              auto overload = find_overload(*f->get_function(),
                parameters, node.get_scope());
              if(overload == nullptr) {
                throw syntax_error(syntax_error_code::OVERLOAD_NOT_FOUND,
                  node.get_callable().get_location());
              }
              return std::static_pointer_cast<data_type>(overload->get_type());
            } else {
              return deduce_data_type(node.get_callable(), *m_type_map);
            }
          }();
        if(auto f = std::dynamic_pointer_cast<function_data_type>(
            callable_type)) {
          m_result = f->get_return_type();
        } else {
          throw syntax_error(syntax_error_code::EXPRESSION_NOT_CALLABLE,
            node.get_callable().get_location());
        }
      }

      void visit(const enum_expression& node) override {
        m_result = node.get_enum();
      }

      void visit(const function_expression& node) override {

        // TODO: Add a custom type to represent overloaded functions.
      }

      void visit(const literal_expression& node) override {
        m_result = node.get_literal().get_type();
      }

      void visit(const syntax_node& node) override {
        throw syntax_error(syntax_error_code::EXPRESSION_EXPECTED,
          node.get_location());
      }

      void visit(const variable_expression& node) override {
        auto i = m_type_map->find(node.get_variable());
        if(i == m_type_map->end()) {
          throw syntax_error(syntax_error_code::VARIABLE_NOT_FOUND,
            node.get_location());
        }
        m_result = i->second;
      }
    };
    return deduction_visitor()(e, t);
  }
}

#endif
