#ifndef DARCEL_REACTOR_TRANSLATOR_HPP
#define DARCEL_REACTOR_TRANSLATOR_HPP
#include <memory>
#include <string>
#include <unordered_map>
#include "darcel/reactors/constant_reactor.hpp"
#include "darcel/reactors/reactors.hpp"
#include "darcel/syntax/syntax_nodes.hpp"
#include "darcel/syntax/syntax_node_visitor.hpp"

namespace darcel {

  //! Implements a syntax visitor to translate a node into a reactor.
  class reactor_translator : private syntax_node_visitor {
    public:
      std::shared_ptr<base_reactor> operator ()(const syntax_node& node);

      void visit(const bind_variable_statement& node) override final;

      void visit(const call_expression& node) override final;

      void visit(const literal_expression& node) override final;

      void visit(const variable_expression& node) override final;

    private:
      std::unordered_map<std::shared_ptr<variable>,
        std::shared_ptr<base_reactor>> m_variables;
      std::shared_ptr<base_reactor> m_reactor;
  };

  //! Builds a reactor from a syntax node.
  /*!
    \param node The node to translate into a reactor.
    \return The reactor represented by the syntax node.
  */
  inline std::shared_ptr<base_reactor> translate(const syntax_node& node) {
    return reactor_translator()(node);
  }

  inline std::shared_ptr<base_reactor> reactor_translator::operator ()(
      const syntax_node& node) {
    return m_reactor;
  }

  inline void reactor_translator::visit(const bind_variable_statement& node) {
    node.get_expression().apply(*this);
    m_variables[node.get_variable()] = m_reactor;
  }

  inline void reactor_translator::visit(const call_expression& node) {
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
    m_reactor = std::move(v.m_reactor);
  }

  inline void reactor_translator::visit(const variable_expression& node) {
    m_reactor = m_variables[node.get_variable()];
  }
}

#endif
