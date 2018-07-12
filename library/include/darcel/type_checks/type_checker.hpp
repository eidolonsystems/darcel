#ifndef DARCEL_TYPE_CHECKER_HPP
#define DARCEL_TYPE_CHECKER_HPP
#include <deque>
#include <unordered_map>
#include "darcel/semantics/scope.hpp"
#include "darcel/semantics/variable.hpp"
#include "darcel/syntax/syntax_node_visitor.hpp"
#include "darcel/syntax/syntax_nodes.hpp"
#include "darcel/syntax/variable_not_found_error.hpp"
#include "darcel/type_checks/function_overloads.hpp"
#include "darcel/type_checks/type_checks.hpp"

namespace darcel {

  //! Performs type checking on syntax nodes.
  class type_checker {
    public:

      //! Constructs a type checker.
      type_checker();

      //! Constructs a type checker.
      /*!
        \param s The top level scope.
      */
      type_checker(const scope& s);

      //! Returns the data type an expression evaluates to.
      std::shared_ptr<data_type> get_type(const expression& e) const;

      //! Returns a variable's data type.
      std::shared_ptr<data_type> get_type(const variable& v) const;

      //! Returns a statement's definition.
      const std::shared_ptr<function_definition>& get_definition(
        const bind_function_statement& s) const;

      //! Returns an expression's particular overload.
      const std::shared_ptr<function_definition>& get_definition(
        const expression& e) const;

      //! Type checks a syntax node.
      void check(const syntax_node& node);

    private:
      struct call_entry {
        std::shared_ptr<function_definition> m_definition;
        std::shared_ptr<function_data_type> m_type;
      };
      std::deque<std::unique_ptr<scope>> m_scopes;
      std::unordered_map<const element*, std::shared_ptr<data_type>> m_types;
      std::unordered_map<const bind_function_statement*,
        std::shared_ptr<function_definition>> m_definitions;
      std::unordered_map<const expression*, call_entry> m_call_entries;
  };

  inline type_checker::type_checker() {
    m_scopes.push_back(std::make_unique<scope>());
  }

  inline type_checker::type_checker(const scope& s) {
    m_scopes.push_back(std::make_unique<scope>(&s));
  }

  inline std::shared_ptr<data_type> type_checker::get_type(
      const expression& e) const {
    struct type_deduction_visitor final : syntax_node_visitor {
      const type_checker* m_checker;
      std::shared_ptr<data_type> m_result;

      std::shared_ptr<data_type> operator ()(const type_checker& checker,
          const expression& node) {
        m_checker = &checker;
        node.apply(*this);
        return std::move(m_result);
      }

      void visit(const call_expression& node) override {
        auto callable_type = m_checker->get_type(node.get_callable());
        if(auto t = std::dynamic_pointer_cast<function_data_type>(
            callable_type)) {
          m_result = t->get_return_type();
        } else {
          visit(static_cast<const expression&>(node));
        }
      }

      void visit(const enum_expression& node) override {
        m_result = node.get_enum();
      }

      void visit(const function_expression& node) override {
        auto entry = m_checker->m_call_entries.find(&node);
        if(entry == m_checker->m_call_entries.end()) {
          auto i = m_checker->m_types.find(node.get_function().get());
          if(i == m_checker->m_types.end()) {
            m_result = std::make_shared<callable_data_type>(
              node.get_function());
          } else {
            m_result = i->second;
          }
        } else {
          m_result = entry->second.m_type;
        }
      }

      void visit(const literal_expression& node) override {
        m_result = node.get_literal().get_type();
      }

      void visit(const syntax_node& node) override {
        throw syntax_error(syntax_error_code::EXPRESSION_EXPECTED,
          node.get_location());
      }

      void visit(const variable_expression& node) override {
        auto entry = m_checker->m_call_entries.find(&node);
        if(entry != m_checker->m_call_entries.end()) {
          m_result = entry->second.m_type;
        } else {
          auto i = m_checker->m_types.find(node.get_variable().get());
          if(i == m_checker->m_types.end()) {
            throw variable_not_found_error(node.get_location(),
              node.get_variable()->get_name());
          }
          m_result = i->second;
        }
      }
    };
    return type_deduction_visitor()(*this, e);
  }

  inline std::shared_ptr<data_type> type_checker::get_type(
      const variable& v) const {
    auto i = m_types.find(&v);
    if(i == m_types.end()) {
      throw variable_not_found_error(v.get_location(), v.get_name());
    }
    return i->second;
  }

  inline const std::shared_ptr<function_definition>&
      type_checker::get_definition(const bind_function_statement& s) const {
    auto i = m_definitions.find(&s);
    if(i == m_definitions.end()) {
      static const std::shared_ptr<function_definition> NONE;
      return NONE;
    }
    return i->second;
  }

  inline const std::shared_ptr<function_definition>&
      type_checker::get_definition(const expression& e) const {
    auto i = m_call_entries.find(&e);
    if(i == m_call_entries.end()) {
      static const std::shared_ptr<function_definition> NONE;
      return NONE;
    }
    return i->second.m_definition;
  }

  inline void type_checker::check(const syntax_node& node) {
    struct type_check_visitor final : syntax_node_visitor {
      type_checker* m_checker;
      std::shared_ptr<data_type> m_last;

      void operator ()(type_checker& checker, const syntax_node& node) {
        m_checker = &checker;
        node.apply(*this);
      }

      void visit(const bind_function_statement& node) override {
        std::vector<function_data_type::parameter> parameters;
        for(auto& parameter : node.get_parameters()) {
          parameters.emplace_back(parameter.m_variable->get_name(),
            *parameter.m_type);
          m_checker->m_types.insert(
            std::make_pair(parameter.m_variable.get(), *parameter.m_type));
        }
        m_checker->m_scopes.back()->add(node.get_function());
        node.get_expression().apply(*this);
        if(m_checker->m_types.find(node.get_function().get()) ==
            m_checker->m_types.end()) {
          auto callable_type = std::make_shared<callable_data_type>(
            node.get_function());
          m_checker->m_types.insert(std::make_pair(node.get_function().get(),
            callable_type));
          m_checker->m_scopes.back()->add(callable_type);
        }
        auto t = std::make_shared<function_data_type>(std::move(parameters),
          std::move(m_last));
        auto definition = std::make_shared<function_definition>(
          node.get_location(), node.get_function(), std::move(t));
        m_checker->m_definitions.insert(std::make_pair(&node, definition));
        m_checker->m_scopes.back()->add(definition);
      }

      void visit(const bind_variable_statement& node) override {
        node.get_expression().apply(*this);
        m_checker->m_types.insert(
          std::make_pair(node.get_variable().get(), std::move(m_last)));
      }

      void visit(const call_expression& node) override {
        node.get_callable().apply(*this);
        auto t = std::move(m_last);
        if(auto callable_type =
            std::dynamic_pointer_cast<callable_data_type>(t)) {
          std::vector<function_data_type::parameter> parameters;
          for(auto& parameter : node.get_parameters()) {
            parameter->apply(*this);
            parameters.emplace_back("", std::move(m_last));
          }
          auto overload = find_overload(*callable_type->get_function(),
            parameters, *m_checker->m_scopes.back());
          if(overload == nullptr) {
            throw syntax_error(syntax_error_code::OVERLOAD_NOT_FOUND,
              node.get_callable().get_location());
          }
          auto instance = instantiate(*overload, parameters,
            *m_checker->m_scopes.back());
          for(std::size_t i = 0; i < parameters.size(); ++i) {
            auto& p = parameters[i].m_type;
            auto& o = instance->get_parameters()[i].m_type;
            if(auto callable_type =
                std::dynamic_pointer_cast<callable_data_type>(p)) {
              if(auto signature =
                  std::dynamic_pointer_cast<function_data_type>(o)) {
                auto call_overload = find_overload(
                  *callable_type->get_function(), *signature,
                  *m_checker->m_scopes.back());
                m_checker->m_call_entries.insert(
                  std::make_pair(node.get_parameters()[i].get(),
                  type_checker::call_entry{call_overload, signature}));
              }
            }
          }
          type_checker::call_entry entry{std::move(overload),
            std::move(instance)};
          m_checker->m_call_entries.insert(
            std::make_pair(&node.get_callable(), std::move(entry)));
        }
        visit(static_cast<const expression&>(node));
      }

      void visit(const expression& node) override {
        m_last = m_checker->get_type(node);
      }

      void visit(const function_expression& node) override {
        m_last = std::make_shared<callable_data_type>(node.get_function());
      }
    };
    type_check_visitor()(*this, node);
  }
}

#endif
