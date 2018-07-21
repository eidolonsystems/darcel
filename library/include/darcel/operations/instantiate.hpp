#ifndef DARCEL_OPERATIONS_INSTANTIATE_HPP
#define DARCEL_OPERATIONS_INSTANTIATE_HPP
#include <memory>
#include "darcel/data_types/function_data_type.hpp"
#include "darcel/operations/clone_structure.hpp"
#include "darcel/syntax/syntax_nodes.hpp"
#include "darcel/syntax/syntax_node_visitor.hpp"

namespace darcel {

  //! Instantiates a generic function definition.
  /*!
    \param node The definition to instantiate.
    \param t The type to instantiate with.
  */
  inline std::unique_ptr<bind_function_statement> instantiate(
      const bind_function_statement& node, const FunctionDataType& t) {
    struct InstantiateVisitor final : syntax_node_visitor {
      std::unordered_map<std::shared_ptr<Variable>, std::shared_ptr<Variable>>
        m_substitutions;
      std::unique_ptr<syntax_node> m_clone;

      std::unique_ptr<bind_function_statement> operator ()(
          const bind_function_statement& node, const FunctionDataType& t) {
        std::vector<bind_function_statement::parameter> parameters;
        for(std::size_t i = 0; i < node.get_parameters().size(); ++i) {
          auto& p = node.get_parameters()[i];
          auto substitution = std::make_shared<Variable>(
            p.m_variable->get_location(), p.m_variable->get_name());
          parameters.emplace_back(substitution, t.get_parameters()[i].m_type);
          m_substitutions.insert(std::make_pair(p.m_variable,
            std::move(substitution)));
        }
        node.get_expression().apply(*this);
        auto definition = static_pointer_cast<expression>(std::move(m_clone));
        auto instantiation = std::make_unique<bind_function_statement>(
          node.get_location(), node.get_function(), std::move(parameters),
          std::move(definition));
        return instantiation;
      }

      void visit(const bind_variable_statement& node) override {
        auto v = std::make_shared<Variable>(node.get_variable()->get_location(),
          node.get_variable()->get_name());
        m_substitutions.insert(std::make_pair(node.get_variable(), v));
        node.get_expression().apply(*this);
        auto e = static_pointer_cast<expression>(std::move(m_clone));
        m_clone = std::make_unique<bind_variable_statement>(node.get_location(),
          std::move(v), std::move(e));
      }

      void visit(const call_expression& node) override {
        node.get_callable().apply(*this);
        auto callable = static_pointer_cast<expression>(std::move(m_clone));
        std::vector<std::unique_ptr<expression>> arguments;
        for(auto& argument : node.get_arguments()) {
          argument->apply(*this);
          arguments.push_back(static_pointer_cast<expression>(
            std::move(m_clone)));
        }
        m_clone = std::make_unique<call_expression>(node.get_location(),
          std::move(callable), std::move(arguments));
      }

      void visit(const syntax_node& node) override {
        m_clone = clone_structure(node);
      }

      void visit(const variable_expression& node) override {
        auto v = m_substitutions.find(node.get_variable());
        if(v == m_substitutions.end()) {
          m_clone = std::make_unique<variable_expression>(node.get_location(),
            node.get_variable());
        } else {
          m_clone = std::make_unique<variable_expression>(node.get_location(),
            v->second);
        }
      }
    };
    return InstantiateVisitor()(node, t);
  }
}

#endif
