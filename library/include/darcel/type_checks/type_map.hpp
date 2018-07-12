#ifndef DARCEL_TYPE_MAP_HPP
#define DARCEL_TYPE_MAP_HPP
#include <memory>
#include <unordered_map>
#include "darcel/data_types/function_data_type.hpp"
#include "darcel/semantics/semantics.hpp"
#include "darcel/syntax/syntax_nodes.hpp"
#include "darcel/type_checks/type_checks.hpp"

namespace darcel {

  //! Stores a map from expressions/variables to their data type.
  class type_map {
    public:

      //! Constructs an empty type_map.
      type_map() = default;

      //! Returns the data type an expression evaluates to.
      std::shared_ptr<data_type> get_type(const expression& e) const;

      //! Returns a function's data type.
      std::shared_ptr<data_type> get_type(const function& f) const;

      //! Returns a variable's data type.
      std::shared_ptr<data_type> get_type(const variable& v) const;

      //! Records a function's data type.
      void add(const function& f, std::shared_ptr<data_type> t);

      //! Records a variable's data type.
      void add(const variable& v, std::shared_ptr<data_type> t);

      //! Records an expression's data type.
      void add(const expression& e, std::shared_ptr<data_type> t);

    private:
      std::unordered_map<const element*, std::shared_ptr<data_type>>
        m_definitions;
      std::unordered_map<const expression*, std::shared_ptr<data_type>>
        m_expressions;
  };

  inline std::shared_ptr<data_type> type_map::get_type(
      const expression& e) const {
    struct type_deduction_visitor final : syntax_node_visitor {
      const type_map* m_types;
      std::shared_ptr<data_type> m_result;

      std::shared_ptr<data_type> operator ()(const type_map& types,
          const expression& node) {
        m_types = &types;
        node.apply(*this);
        return std::move(m_result);
      }

      void visit(const call_expression& node) override {
        auto callable_type = m_types->get_type(node.get_callable());
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
        auto i = m_types->m_definitions.find(node.get_function().get());
        if(i == m_types->m_definitions.end()) {
          m_result = std::make_shared<callable_data_type>(node.get_function());
        } else {
          m_result = i->second;
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
        auto i = m_types->m_definitions.find(node.get_variable().get());
        if(i == m_types->m_definitions.end()) {
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

  inline std::shared_ptr<data_type> type_map::get_type(
      const function& f) const {
    auto i = m_definitions.find(&f);
    if(i == m_definitions.end()) {
      return nullptr;
    }
    return i->second;
  }

  inline std::shared_ptr<data_type> type_map::get_type(
      const variable& v) const {
    auto i = m_definitions.find(&v);
    if(i == m_definitions.end()) {
      return nullptr;
    }
    return i->second;
  }

  inline void type_map::add(const function& f, std::shared_ptr<data_type> t) {
    m_definitions.insert(std::make_pair(&f, std::move(t)));
  }

  inline void type_map::add(const variable& v, std::shared_ptr<data_type> t) {
    m_definitions.insert(std::make_pair(&v, std::move(t)));
  }

  inline void type_map::add(const expression& e, std::shared_ptr<data_type> t) {
    m_expressions.insert(std::make_pair(&e, std::move(t)));
  }
}

#endif
