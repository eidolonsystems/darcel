#ifndef DARCEL_PRETTY_PRINT_HPP
#define DARCEL_PRETTY_PRINT_HPP
#include <iostream>
#include "darcel/syntax/syntax_nodes.hpp"
#include "darcel/syntax/syntax_node_visitor.hpp"

namespace darcel {

  //! Pretty prints a syntax node to an output stream.
  /*!
    \param node The syntax node to print.
    \param out The output stream to print to.
  */
  inline void pretty_print(const syntax_node& node, std::ostream& out) {
    struct pretty_print_visitor : syntax_node_visitor {
      int m_indent_level;
      std::ostream* m_out;

      void operator ()(const syntax_node& node, std::ostream& out) {
        m_indent_level = 0;
        m_out = &out;
        node.apply(*this);
      }

      void visit(const bind_enum_statement& node) override final {
        *m_out << "let " << node.get_enum()->get_name() << " = enum(\n";
        ++m_indent_level;
        auto next_value = 0;
        auto is_first = true;
        for(std::size_t i = 0;
            i != node.get_enum()->get_symbols().size(); ++i) {
          auto& e = node.get_enum()->get_symbols()[i];
          indent();
          *m_out << e.m_name;
          if(e.m_value != next_value) {
            *m_out << " = " << e.m_value;
          }
          next_value = e.m_value + 1;
          if(i != node.get_enum()->get_symbols().size() - 1) {
            *m_out << ",\n";
          }
        }
        *m_out << ")";
        --m_indent_level;
      }

      void visit(const bind_function_statement& node) override final {
        *m_out << "let " << node.get_function()->get_name() << "(";
        auto is_first = true;
        for(auto& parameter : node.get_parameters()) {
          if(!is_first) {
            *m_out << ", ";
          } else {
            is_first = false;
          }
          if(!parameter->get_name().empty()) {
            *m_out << parameter->get_name() << ": ";
          }
          *m_out << parameter->get_data_type()->get_name();
        }
        *m_out << ") = ";
        node.get_expression().apply(*this);
      }

      void visit(const bind_variable_statement& node) override final {
        *m_out << "let " << node.get_variable()->get_name() << " = ";
        node.get_expression().apply(*this);
      }

      void visit(const call_expression& node) override final {
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

      void visit(const enum_expression& node) override final {
        auto e = node.get_enum();
        *m_out << e->get_name() << "." <<
          e->get_symbols()[node.get_index()].m_name;
      }

      void visit(const function_expression& node) override final {
        *m_out << node.get_function()->get_name();
      }

      void visit(const literal_expression& node) override final {
        if(*node.get_literal().get_type() == *text_data_type::get_instance()) {
          *m_out << '"' << node.get_literal().get_value() << '"';
        } else {
          *m_out << node.get_literal().get_value();
        }
      }

      void visit(const variable_expression& node) override final {
        *m_out << node.get_variable()->get_name();
      }

      void indent() {
        for(auto i = 0; i < m_indent_level; ++i) {
          *m_out << "  ";
        }
      }
    };
    pretty_print_visitor()(node, out);
  }

  //! Pretty prints a syntax node to stdout.
  /*!
    \param node The syntax node to print.
  */
  inline void pretty_print(const syntax_node& node) {
    pretty_print(node, std::cout);
  }
}

#endif
