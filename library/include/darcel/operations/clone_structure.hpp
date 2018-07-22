#ifndef DARCEL_CLONE_STRUCTURE_HPP
#define DARCEL_CLONE_STRUCTURE_HPP
#include "darcel/syntax/syntax_nodes.hpp"
#include "darcel/syntax/syntax_node_visitor.hpp"
#include "darcel/utilities/utilities.hpp"

namespace darcel {

  //! Copies a node structurally, that is, the AST nodes are cloned but the
  //! elements they refer to are not.
  /*!
    \param node The node to clone.
    \return A structural clone of the node.
  */
  template<typename T>
  std::unique_ptr<T> clone_structure(const T& node) {
    struct CloneVisitor final : SyntaxNodeVisitor {
      std::unique_ptr<SyntaxNode> m_clone;

      std::unique_ptr<T> operator ()(const T& node) {
        node.apply(*this);
        return static_pointer_cast<T>(std::move(m_clone));
      }

      void visit(const BindEnumStatement& node) override {
        m_clone = std::make_unique<BindEnumStatement>(node.get_enum());
      }

      void visit(const BindFunctionStatement& node) override {
        m_clone = std::make_unique<BindFunctionStatement>(node.get_location(),
          node.get_function(), node.get_parameters(),
          clone_structure(node.get_expression()));
      }

      void visit(const BindVariableStatement& node) override {
        m_clone = std::make_unique<BindVariableStatement>(node.get_location(),
          node.get_variable(), clone_structure(node.get_expression()));
      }

      void visit(const CallExpression& node) override {
        m_clone = std::make_unique<CallExpression>(node.get_location(),
          clone_structure(node.get_callable()),
          clone_structure(node.get_arguments()));
      }

      void visit(const EnumExpression& node) override {
        m_clone = std::make_unique<EnumExpression>(node.get_location(),
          node.get_enum(), node.get_index());
      }

      void visit(const FunctionExpression& node) override {
        m_clone = std::make_unique<FunctionExpression>(node.get_location(),
        node.get_function());
      }

      void visit(const LiteralExpression& node) override {
        m_clone = std::make_unique<LiteralExpression>(node.get_location(),
          node.get_literal());
      }

      void visit(const VariableExpression& node) override {
        m_clone = std::make_unique<VariableExpression>(node.get_location(),
          node.get_variable());
      }
    };
    return CloneVisitor()(node);
  }

  template<typename T>
  std::vector<std::unique_ptr<T>> clone_structure(
      const std::vector<std::unique_ptr<T>>& nodes) {
    std::vector<std::unique_ptr<T>> clones;
    for(auto& node : nodes) {
      clones.push_back(clone_structure(*node));
    }
    return clones;
  }
}

#endif
