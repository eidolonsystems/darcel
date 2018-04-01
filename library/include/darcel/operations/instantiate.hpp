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
      const std::unordered_map<std::shared_ptr<variable>,
      std::shared_ptr<function>>& functions) {
    struct instantiate_visitor : syntax_node_visitor {
      const std::unordered_map<std::shared_ptr<variable>,
        std::shared_ptr<function>>* m_functions;
      std::unordered_map<std::shared_ptr<variable>,
        std::shared_ptr<variable>> m_substitutions;
      std::unique_ptr<syntax_node> m_clone;

      std::unique_ptr<bind_function_statement> operator ()(
          const bind_function_statement& node,
          std::shared_ptr<variable> overload,
          const std::unordered_map<std::shared_ptr<variable>,
          std::shared_ptr<function>>& functions) {
        m_functions = &functions;
        auto type = std::static_pointer_cast<function_data_type>(
          overload->get_data_type());
        std::vector<std::shared_ptr<variable>> substitutions;
        for(std::size_t i = 0; i < node.get_parameters().size(); ++i) {
          auto& p = node.get_parameters()[i];
          auto& q = type->get_parameters()[i].m_type;
          auto substitution = std::make_shared<variable>(p->get_location(),
            p->get_name(), q);
          substitutions.push_back(substitution);
          m_substitutions.insert(std::make_pair(p, std::move(substitution)));
        }
        node.get_expression().apply(*this);
        auto definition = std::unique_ptr<expression>(static_cast<expression*>(
          m_clone.release()));
        auto instantiation = std::make_unique<bind_function_statement>(
          node.get_location(), node.get_function(), std::move(overload),
          std::move(substitutions), std::move(definition));
        return instantiation;
      }

      void visit(const bind_function_statement& node) override final {
/* TODO
        m_clone = std::make_unique<bind_function_statement>(node.get_location(),
          node.get_function(), node.get_overload(), node.get_parameters(),
          clone_structure(node.get_expression()));
*/
      }

      void visit(const bind_variable_statement& node) override final {
        auto v = [&] {
          auto v = m_substitutions.find(node.get_variable());
          if(v == m_substitutions.end()) {
            return node.get_variable();
          } else {
            return v->second;
          }
        }();
        node.get_expression().apply(*this);
        auto e = std::unique_ptr<expression>(
          static_cast<expression*>(m_clone.release()));
        m_clone = std::make_unique<bind_variable_statement>(node.get_location(),
          v, std::move(e));
      }

      void visit(const call_expression& node) override final {
        std::vector<std::unique_ptr<expression>> arguments;
        for(auto& p : node.get_parameters()) {
          p->apply(*this);
          arguments.push_back(std::unique_ptr<expression>(
            static_cast<expression*>(m_clone.release())));
        }
        std::optional<variable_expression> substitution;
        auto& c = [&] () -> const expression& {
          if(is_generic(*node.get_callable().get_data_type())) {
            auto& v = static_cast<const variable_expression&>(
              node.get_callable());
            auto& f = m_functions->at(v.get_variable());
            std::vector<function_data_type::parameter> parameters;
            for(auto& argument : arguments) {
              parameters.emplace_back("", argument->get_data_type());
            }
            auto o = find_overload(*f, parameters);
            substitution.emplace(node.get_callable().get_location(), o);
            return *substitution;
          } else {
            return node.get_callable();
          }
        }();
        c.apply(*this);
        auto callable = std::unique_ptr<expression>(
          static_cast<expression*>(m_clone.release()));
        m_clone = std::make_unique<call_expression>(node.get_location(),
          std::move(callable), std::move(arguments));
      }

      void visit(const function_expression& node) override final {
        m_clone = std::make_unique<function_expression>(node.get_location(),
          node.get_function());
      }

      void visit(const literal_expression& node) override final {
        m_clone = std::make_unique<literal_expression>(node.get_location(),
          node.get_literal());
      }

      void visit(const variable_expression& node) override final {
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
    return instantiate_visitor()(node, std::move(overload), functions);
  }
}

#endif
