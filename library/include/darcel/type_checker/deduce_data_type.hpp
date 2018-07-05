#ifndef DARCEL_DEDUCE_DATA_TYPE_HPP
#define DARCEL_DEDUCE_DATA_TYPE_HPP
#include "darcel/syntax/syntax_node_visitor.hpp"
#include "darcel/syntax/syntax_nodes.hpp"
#include "darcel/type_checker/type_checker.hpp"

namespace darcel {

  //! Deduces an expression's data type.
  /*!
    \param e The expression whose data type is to be deduced.
    \param t A map containing previously deduced expression data types.
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
        auto callable_type =
          [&] {
            if(auto f = dynamic_cast<const function_expression*>(
                &node.get_callable())) {
              return deduce_data_type(node.get_callable(), *m_type_map);
            } else {
              return deduce_data_type(node.get_callable(), *m_type_map);
            }
          }();
        std::vector<std::shared_ptr<data_type>> parameters;
        for(auto& parameter : node.get_parameters()) {
          parameters.push_back(deduce_data_type(*parameter, *m_type_map));
        }
      }

      void visit(const enum_expression& node) override {
        m_result = node.get_enum();
      }

      void visit(const function_expression& node) override {
      }

      void visit(const literal_expression& node) override {
        m_result = node.get_literal().get_type();
      }

      void visit(const variable_expression& node) override {
        auto i = m_type_map->find(node.get_variable());
        if(i == m_type_map->end()) {
          return;
        }
        m_result = i->second;
      }
    };
    return deduction_visitor()(e, t);
  }
}

#endif
