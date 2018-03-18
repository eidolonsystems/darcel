#ifndef DARCEL_PRETTY_PRINT_HPP
#define DARCEL_PRETTY_PRINT_HPP
#include <iostream>
#include "darcel/syntax/syntax_nodes.hpp"
#include "darcel/syntax/syntax_node_visitor.hpp"

namespace darcel {

  //! Implements a visitor to print syntax nodes.
  class pretty_print_visitor : private syntax_node_visitor {
    public:
      void operator ()(const syntax_node& node, std::ostream& out);

      void visit(const bind_variable_statement& node) override final;

      void visit(const call_expression& node) override final;

      void visit(const literal_expression& node) override final;

      void visit(const variable_expression& node) override final;

    private:
      int m_indent_level;
      std::ostream* m_out;

      void indent();
  };

  //! Pretty prints a syntax node to an output stream.
  /*!
    \param node The syntax node to print.
    \param out The output stream to print to.
  */
  inline void pretty_print(const syntax_node& node, std::ostream& out) {
    pretty_print_visitor()(node, out);
  }

  //! Pretty prints a syntax node to stdout.
  /*!
    \param node The syntax node to print.
  */
  inline void pretty_print(const syntax_node& node) {
    pretty_print(node, std::cout);
  }

  inline void pretty_print_visitor::operator ()(const syntax_node& node,
      std::ostream& out) {
    m_indent_level = 0;
    m_out = &out;
    node.apply(*this);
  }

  inline void pretty_print_visitor::visit(const bind_variable_statement& node) {
    *m_out << "let " << node.get_variable()->get_name() << " = ";
    node.get_expression().apply(*this);
  }

  inline void pretty_print_visitor::visit(const call_expression& node) {
    node.get_callable().apply(*this);
    *m_out << '(';
    auto is_first = true;
    for(auto& parameter : node.get_parameters()) {
      if(!is_first) {
        *m_out << ", ";
      } else {
        is_first = false;
      }
      parameter->apply(*this);
    }
    *m_out << ')';
  }

  inline void pretty_print_visitor::visit(const literal_expression& node) {
    if(*node.get_literal().get_type() == *text_data_type::get_instance()) {
      *m_out << '"' << node.get_literal().get_value() << '"';
    } else {
      *m_out << node.get_literal().get_value();
    }
  }

  inline void pretty_print_visitor::visit(const variable_expression& node) {
    *m_out << node.get_variable()->get_name();
  }

  inline void pretty_print_visitor::indent() {
    for(auto i = 0; i < m_indent_level; ++i) {
      *m_out << "  ";
    }
  }
}

#endif
