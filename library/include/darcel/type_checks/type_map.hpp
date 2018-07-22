#ifndef DARCEL_TYPE_MAP_HPP
#define DARCEL_TYPE_MAP_HPP
#include <deque>
#include <memory>
#include <unordered_map>
#include "darcel/data_types/function_data_type.hpp"
#include "darcel/data_types/generic_data_type.hpp"
#include "darcel/semantics/semantics.hpp"
#include "darcel/syntax/syntax_nodes.hpp"
#include "darcel/type_checks/function_overloads.hpp"
#include "darcel/type_checks/type_checks.hpp"

namespace darcel {

  //! Stores a map from expressions/variables to their data type.
  class TypeMap {
    public:

      //! Constructs an empty TypeMap.
      TypeMap() = default;

      //! Returns the data type an expression evaluates to.
      std::shared_ptr<DataType> get_type(const Expression& e) const;

      //! Returns a function's data type.
      std::shared_ptr<DataType> get_type(const Function& f) const;

      //! Returns a variable's data type.
      std::shared_ptr<DataType> get_type(const Variable& v) const;

      //! Records a function's data type.
      void add(const Function& f, std::shared_ptr<DataType> t);

      //! Adds a function definition.
      void add(std::shared_ptr<FunctionDefinition> definition);

      //! Records a variable's data type.
      void add(const Variable& v, std::shared_ptr<DataType> t);

      //! Records an expression's data type.
      void add(const Expression& e, std::shared_ptr<DataType> t);

    private:
      std::unordered_map<const Element*, std::shared_ptr<DataType>> m_types;
      std::unordered_map<const Function*,
        std::vector<std::shared_ptr<FunctionDefinition>>> m_definitions;
      std::unordered_map<const Expression*, std::shared_ptr<DataType>>
        m_expressions;

      std::deque<std::unique_ptr<Scope>> build_scope(
        std::shared_ptr<Function> f) const;
  };

  inline std::shared_ptr<DataType> TypeMap::get_type(
      const Expression& e) const {
    struct type_deduction_visitor final : SyntaxNodeVisitor {
      const TypeMap* m_types;
      std::shared_ptr<DataType> m_result;

      std::shared_ptr<DataType> operator ()(const TypeMap& types,
          const Expression& node) {
        m_types = &types;
        node.apply(*this);
        return std::move(m_result);
      }

      void visit(const CallExpression& node) override {
        auto t = m_types->get_type(node.get_callable());
        if(auto f = std::dynamic_pointer_cast<FunctionDataType>(t)) {
          m_result = f->get_return_type();
        } else if(auto f = std::dynamic_pointer_cast<CallableDataType>(t)) {
          std::vector<FunctionDataType::Parameter> parameters;
          for(auto& argument : node.get_arguments()) {
            auto argument_type = m_types->get_type(*argument);
            if(argument_type == nullptr) {
              m_result = nullptr;
              return;
            }
            parameters.emplace_back("", std::move(argument_type));
          }
          auto s = m_types->build_scope(f->get_function());
          auto overload = find_overload(*f->get_function(), parameters,
            *s.back());
          if(overload == nullptr) {
            m_result = nullptr;
          } else {
            auto instance = instantiate(*overload, parameters, *s.back());
            m_result = instance->get_return_type();
          }
        } else {
          visit(static_cast<const Expression&>(node));
        }
      }

      void visit(const EnumExpression& node) override {
        m_result = node.get_enum();
      }

      void visit(const FunctionExpression& node) override {
        auto i = m_types->m_types.find(node.get_function().get());
        if(i == m_types->m_types.end()) {
          m_result = std::make_shared<CallableDataType>(node.get_function());
        } else {
          m_result = i->second;
        }
      }

      void visit(const LiteralExpression& node) override {
        m_result = node.get_literal().get_type();
      }

      void visit(const SyntaxNode& node) override {
        throw SyntaxError(SyntaxErrorCode::EXPRESSION_EXPECTED,
          node.get_location());
      }

      void visit(const VariableExpression& node) override {
        auto i = m_types->m_types.find(node.get_variable().get());
        if(i == m_types->m_types.end()) {
          m_result = nullptr;
        } else {
          m_result = i->second;
        }
      }
    };
    auto i = m_expressions.find(&e);
    if(i != m_expressions.end()) {
      return i->second;
    }
    return type_deduction_visitor()(*this, e);
  }

  inline std::shared_ptr<DataType> TypeMap::get_type(
      const Function& f) const {
    auto i = m_types.find(&f);
    if(i == m_types.end()) {
      return nullptr;
    }
    return i->second;
  }

  inline std::shared_ptr<DataType> TypeMap::get_type(
      const Variable& v) const {
    auto i = m_types.find(&v);
    if(i == m_types.end()) {
      return nullptr;
    }
    return i->second;
  }

  inline void TypeMap::add(const Function& f, std::shared_ptr<DataType> t) {
    m_types[&f] = std::move(t);
  }

  inline void TypeMap::add(std::shared_ptr<FunctionDefinition> definition) {
    m_definitions[definition->get_function().get()].push_back(
      std::move(definition));
  }

  inline void TypeMap::add(const Variable& v, std::shared_ptr<DataType> t) {
    m_types[&v] = std::move(t);
  }

  inline void TypeMap::add(const Expression& e, std::shared_ptr<DataType> t) {
    m_expressions[&e] = std::move(t);
  }

  inline std::deque<std::unique_ptr<Scope>> TypeMap::build_scope(
      std::shared_ptr<Function> f) const {
    std::deque<std::unique_ptr<Scope>> s;
    std::deque<std::shared_ptr<Function>> hierarchy;
    while(f != nullptr) {
      hierarchy.push_back(f);
      f = f->get_parent();
    }
    std::unique_ptr<Scope> level;
    while(!hierarchy.empty()) {
      if(s.empty()) {
        level = std::make_unique<Scope>();
      } else {
        level = std::make_unique<Scope>(s.back().get());
      }
      level->add(hierarchy.back());
      auto definitions = m_definitions.find(hierarchy.back().get());
      if(definitions != m_definitions.end()) {
        for(auto& definition : definitions->second) {
          level->add(definition);
        }
      }
      hierarchy.pop_back();
      s.push_back(std::move(level));
    }
    return s;
  }
}

#endif
