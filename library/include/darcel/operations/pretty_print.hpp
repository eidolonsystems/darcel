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
  inline void pretty_print(const SyntaxNode& node, std::ostream& out) {
    struct PrettyPrintVisitor final : SyntaxNodeVisitor {
      int m_indent_level;
      std::ostream* m_out;

      void operator ()(const SyntaxNode& node, std::ostream& out) {
        m_indent_level = 0;
        m_out = &out;
        node.apply(*this);
      }

      void visit(const BindEnumStatement& node) override {
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

      void visit(const BindFunctionStatement& node) override {
        *m_out << "let " << node.get_function()->get_name() << "(";
        auto is_first = true;
        for(auto& parameter : node.get_parameters()) {
          if(!is_first) {
            *m_out << ", ";
          } else {
            is_first = false;
          }
          *m_out << parameter.m_variable->get_name() << ": ";
          if(parameter.m_type != nullptr) {
            *m_out << parameter.m_type->get_name();
          }
        }
        *m_out << ") = ";
        node.get_expression().apply(*this);
      }

      void visit(const BindVariableStatement& node) override {
        *m_out << "let " << node.get_variable()->get_name() << " = ";
        node.get_expression().apply(*this);
      }

      void visit(const CallExpression& node) override {
        node.get_callable().apply(*this);
        *m_out << '(';
        auto is_first = true;
        for(auto& argument : node.get_arguments()) {
          if(!is_first) {
            *m_out << ", ";
          } else {
            is_first = false;
          }
          argument->apply(*this);
        }
        *m_out << ')';
      }

      void visit(const EnumExpression& node) override {
        auto e = node.get_enum();
        *m_out << e->get_name() << "." <<
          e->get_symbols()[node.get_index()].m_name;
      }

      void visit(const FunctionExpression& node) override {
        *m_out << node.get_function()->get_name();
      }

      void visit(const LiteralExpression& node) override {
        if(*node.get_literal().get_type() == *TextDataType::get_instance()) {
          *m_out << '"' << node.get_literal().get_value() << '"';
        } else {
          *m_out << node.get_literal().get_value();
        }
      }

      void visit(const VariableExpression& node) override {
        *m_out << node.get_variable()->get_name();
      }

      void indent() {
        for(auto i = 0; i < m_indent_level; ++i) {
          *m_out << "  ";
        }
      }
    };
    PrettyPrintVisitor()(node, out);
  }

  //! Pretty prints a syntax node to stdout.
  /*!
    \param node The syntax node to print.
  */
  inline void pretty_print(const SyntaxNode& node) {
    pretty_print(node, std::cout);
  }
}

#endif
