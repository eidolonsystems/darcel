#ifndef DARCEL_REACTOR_TRANSLATOR_HPP
#define DARCEL_REACTOR_TRANSLATOR_HPP
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include "darcel/reactors/chain_reactor.hpp"
#include "darcel/reactors/constant_reactor.hpp"
#include "darcel/reactors/operators.hpp"
#include "darcel/reactors/ostream_reactor.hpp"
#include "darcel/reactors/reactor_builder.hpp"
#include "darcel/reactors/reactors.hpp"
#include "darcel/reactors/trigger.hpp"
#include "darcel/semantics/scope.hpp"
#include "darcel/syntax/syntax_nodes.hpp"
#include "darcel/syntax/syntax_node_visitor.hpp"

namespace darcel {

  //! Implements a syntax visitor to translate a node into a reactor.
  class reactor_translator : private syntax_node_visitor {
    public:

      //! A variant over the types of values produced by an expression.
      using value = std::variant<std::shared_ptr<base_reactor>,
        std::shared_ptr<reactor_builder>>;

      //! Constructs a reactor translator given a scope containing all of the
      //! built-in global definitions.
      /*!
        \param s The scope the node belongs to.
        \param t The trigger used to indicate reactor updates.
      */
      reactor_translator(const scope& s, trigger& t);

      //! Builds a reactor from a syntax node.
      /*!
        \param node The node to translate into a reactor.
      */
      void translate(const syntax_node& node);

      //! Returns the main reactor.
      const std::shared_ptr<base_reactor>& get_main() const;

      void visit(const bind_variable_statement& node) override final;

      void visit(const call_expression& node) override final;

      void visit(const literal_expression& node) override final;

      void visit(const variable_expression& node) override final;

    private:
      const scope* m_scope;
      trigger* m_trigger;
      std::unordered_map<std::shared_ptr<variable>, value> m_variables;
      value m_evaluation;
      std::shared_ptr<base_reactor> m_main;

      std::shared_ptr<base_reactor> evaluate_reactor(const expression& e);
      std::shared_ptr<reactor_builder> evaluate_builder(const expression& e);
  };

  inline reactor_translator::reactor_translator(const scope& s, trigger& t)
      : m_scope(&s),
        m_trigger(&t) {
    auto add = m_scope->find<function>("add");
    if(add != nullptr) {
      for(auto& overload : add->get_overloads()) {
        auto signature = std::static_pointer_cast<function_data_type>(
          overload->get_data_type());
        if(signature->get_parameters().size() == 2) {
          if(*signature->get_parameters()[0].m_type == integer_data_type() &&
              *signature->get_parameters()[1].m_type == integer_data_type()) {
            m_variables[overload] =
              std::make_shared<add_reactor_builder<int, int>>();
          } else if(
              *signature->get_parameters()[0].m_type == float_data_type() &&
              *signature->get_parameters()[1].m_type == float_data_type()) {
            m_variables[overload] =
              std::make_shared<add_reactor_builder<double, double>>();
          } else if(
              *signature->get_parameters()[0].m_type == text_data_type() &&
              *signature->get_parameters()[1].m_type == text_data_type()) {
            m_variables[overload] =
              std::make_shared<add_reactor_builder<std::string, std::string>>();
          }
        }
      }
    }
    auto chain = m_scope->find<function>("chain");
    if(add != nullptr) {
      for(auto& overload : add->get_overloads()) {
        auto signature = std::static_pointer_cast<function_data_type>(
          overload->get_data_type());
        if(signature->get_parameters().size() == 2) {
          if(*signature->get_parameters()[0].m_type == integer_data_type() &&
              *signature->get_parameters()[1].m_type == integer_data_type()) {
            m_variables[overload] =
              std::make_shared<chain_reactor_builder<int>>();
          } else if(
              *signature->get_parameters()[0].m_type == float_data_type() &&
              *signature->get_parameters()[1].m_type == float_data_type()) {
            m_variables[overload] =
              std::make_shared<chain_reactor_builder<double>>();
          } else if(
              *signature->get_parameters()[0].m_type == text_data_type() &&
              *signature->get_parameters()[1].m_type == text_data_type()) {
            m_variables[overload] =
              std::make_shared<chain_reactor_builder<std::string>>();
          }
        }
      }
    }
    auto print = m_scope->find<function>("print");
    if(print != nullptr) {
      for(auto& overload : add->get_overloads()) {
        auto signature = std::static_pointer_cast<function_data_type>(
          overload->get_data_type());
        if(signature->get_parameters().size() == 1) {
          if(*signature->get_parameters()[0].m_type == integer_data_type()) {
            m_variables[overload] =
              std::make_shared<ostream_reactor_builder<int>>(std::cout);
          } else if(*signature->get_parameters()[0].m_type ==
              float_data_type()) {
            m_variables[overload] =
              std::make_shared<ostream_reactor_builder<double>>(std::cout);
          } else if(*signature->get_parameters()[0].m_type ==
              text_data_type()) {
            m_variables[overload] =
              std::make_shared<ostream_reactor_builder<std::string>>(std::cout);
          }
        }
      }
    }
  }

  inline void reactor_translator::translate(const syntax_node& node) {
    node.apply(*this);
  }

  inline const std::shared_ptr<base_reactor>&
      reactor_translator::get_main() const {
    return m_main;
  }

  inline void reactor_translator::visit(const bind_variable_statement& node) {
    auto translation = evaluate_reactor(node.get_expression());
    m_variables[node.get_variable()] = translation;
    if(node.get_variable()->get_name() == "main") {
      m_main = translation;
    }
  }

  inline void reactor_translator::visit(const call_expression& node) {
    auto builder = evaluate_builder(node.get_callable());
    std::vector<std::shared_ptr<base_reactor>> parameters;
    for(auto& parameter : node.get_parameters()) {
      parameters.push_back(evaluate_reactor(*parameter));
    }
    m_evaluation = builder->build(parameters, *m_trigger);
  }

  inline void reactor_translator::visit(const literal_expression& node) {
    struct literal_visitor : data_type_visitor {
      literal m_literal;
      std::shared_ptr<base_reactor> m_reactor;

      literal_visitor(literal l)
          : m_literal(std::move(l)) {}

      void visit(const bool_data_type& type) {
        if(m_literal.get_value() == "true") {
          m_reactor = make_constant_reactor(true);
        } else {
          m_reactor = make_constant_reactor(false);
        }
      }

      void visit(const float_data_type& type) {
        m_reactor = make_constant_reactor(std::stof(m_literal.get_value()));
      }

      void visit(const integer_data_type& type) {
        m_reactor = make_constant_reactor(std::stoi(m_literal.get_value()));
      }

      void visit(const text_data_type& type) {
        m_reactor = make_constant_reactor(m_literal.get_value());
      }
    };
    literal_visitor v(node.get_literal());
    node.get_data_type()->apply(v);
    m_evaluation = std::move(v.m_reactor);
  }

  inline void reactor_translator::visit(const variable_expression& node) {
    m_evaluation = m_variables[node.get_variable()];
  }

  inline std::shared_ptr<base_reactor> reactor_translator::evaluate_reactor(
      const expression& e) {
    e.apply(*this);
    return std::get<std::shared_ptr<base_reactor>>(m_evaluation);
  }

  inline std::shared_ptr<reactor_builder> reactor_translator::evaluate_builder(
      const expression& e) {
    e.apply(*this);
    return std::get<std::shared_ptr<reactor_builder>>(m_evaluation);
  }
}

#endif
