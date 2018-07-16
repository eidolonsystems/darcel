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

      //! Adds a function definition.
      void add(std::shared_ptr<function_definition> definition);

      //! Records a variable's data type.
      void add(const variable& v, std::shared_ptr<data_type> t);

      //! Records an expression's data type.
      void add(const expression& e, std::shared_ptr<data_type> t);

    private:
      std::unordered_map<const element*, std::shared_ptr<data_type>> m_types;
      std::unordered_map<const function*,
        std::vector<std::shared_ptr<function_definition>>> m_definitions;
      std::unordered_map<const expression*, std::shared_ptr<data_type>>
        m_expressions;

      std::deque<std::unique_ptr<scope>> build_scope(
        std::shared_ptr<function> f) const;
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
        auto t = m_types->get_type(node.get_callable());
        if(auto f = std::dynamic_pointer_cast<function_data_type>(t)) {
          m_result = f->get_return_type();
        } else if(auto f = std::dynamic_pointer_cast<callable_data_type>(t)) {
          std::vector<function_data_type::parameter> parameters;
          for(auto& parameter : node.get_parameters()) {
            auto parameter_type = m_types->get_type(*parameter);
            if(parameter_type == nullptr) {
              m_result = nullptr;
              return;
            }
            parameters.emplace_back("", std::move(parameter_type));
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
          visit(static_cast<const expression&>(node));
        }
      }

      void visit(const enum_expression& node) override {
        m_result = node.get_enum();
      }

      void visit(const function_expression& node) override {
        auto i = m_types->m_types.find(node.get_function().get());
        if(i == m_types->m_types.end()) {
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

  inline std::shared_ptr<data_type> type_map::get_type(
      const function& f) const {
    auto i = m_types.find(&f);
    if(i == m_types.end()) {
      return nullptr;
    }
    return i->second;
  }

  inline std::shared_ptr<data_type> type_map::get_type(
      const variable& v) const {
    auto i = m_types.find(&v);
    if(i == m_types.end()) {
      return nullptr;
    }
    return i->second;
  }

  inline void type_map::add(const function& f, std::shared_ptr<data_type> t) {
    m_types[&f] = std::move(t);
  }

  inline void type_map::add(std::shared_ptr<function_definition> definition) {
    m_definitions[definition->get_function().get()].push_back(
      std::move(definition));
  }

  inline void type_map::add(const variable& v, std::shared_ptr<data_type> t) {
    m_types[&v] = std::move(t);
  }

  inline void type_map::add(const expression& e, std::shared_ptr<data_type> t) {
    m_expressions[&e] = std::move(t);
  }

  inline std::deque<std::unique_ptr<scope>> type_map::build_scope(
      std::shared_ptr<function> f) const {
    std::deque<std::unique_ptr<scope>> s;
    std::deque<std::shared_ptr<function>> hierarchy;
    while(f != nullptr) {
      hierarchy.push_back(f);
      f = f->get_parent();
    }
    std::unique_ptr<scope> level;
    while(!hierarchy.empty()) {
      if(s.empty()) {
        level = std::make_unique<scope>();
      } else {
        level = std::make_unique<scope>(s.back().get());
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
