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

      //! Type checks a syntax node.
      void check(const syntax_node& node);

    private:
      std::deque<std::unique_ptr<scope>> m_scopes;
      std::unordered_map<const variable*, std::shared_ptr<data_type>>
        m_variable_types;
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
        std::vector<function_data_type::parameter> parameters;
        for(auto& parameter : node.get_parameters()) {
          parameters.emplace_back("", m_checker->get_type(*parameter));
        }
        auto callable_type =
          [&] {
            if(auto f = dynamic_cast<const function_expression*>(
                &node.get_callable())) {
              auto overload = find_overload(*f->get_function(), parameters,
                *m_checker->m_scopes.back());
              if(overload == nullptr) {
                throw syntax_error(syntax_error_code::OVERLOAD_NOT_FOUND,
                  node.get_callable().get_location());
              }
              return std::static_pointer_cast<data_type>(overload->get_type());
            } else {
              return m_checker->get_type(node.get_callable());
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

      void visit(const literal_expression& node) override {
        m_result = node.get_literal().get_type();
      }

      void visit(const syntax_node& node) override {
        throw syntax_error(syntax_error_code::EXPRESSION_EXPECTED,
          node.get_location());
      }

      void visit(const variable_expression& node) override {
        auto i = m_checker->m_variable_types.find(node.get_variable().get());
        if(i == m_checker->m_variable_types.end()) {
          throw variable_not_found_error(node.get_location(),
            node.get_variable()->get_name());
        }
        m_result = i->second;
      }
    };
    return type_deduction_visitor()(*this, e);
  }

  inline std::shared_ptr<data_type> type_checker::get_type(
      const variable& v) const {
    auto i = m_variable_types.find(&v);
    if(i == m_variable_types.end()) {
      throw variable_not_found_error(v.get_location(), v.get_name());
    }
    return i->second;
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
          m_checker->m_variable_types.insert(
            std::make_pair(parameter.m_variable.get(), *parameter.m_type));
        }
        node.get_expression().apply(*this);
        auto t = std::make_shared<function_data_type>(std::move(parameters),
          std::move(m_last));
//        m_types->insert(std::make_pair(node.get_function().get(), t));
      }

      void visit(const bind_variable_statement& node) override {
        node.get_expression().apply(*this);
        m_checker->m_variable_types.insert(
          std::make_pair(node.get_variable().get(), std::move(m_last)));
      }

      void visit(const expression& node) override {
        m_last = m_checker->get_type(node);
      }
    };
    type_check_visitor()(*this, node);
  }
}

#endif
