#ifndef DARCEL_SCOPE_HPP
#define DARCEL_SCOPE_HPP
#include <memory>
#include <unordered_map>
#include "darcel/data_types/bool_data_type.hpp"
#include "darcel/data_types/float_data_type.hpp"
#include "darcel/data_types/function_data_type.hpp"
#include "darcel/data_types/integer_data_type.hpp"
#include "darcel/data_types/text_data_type.hpp"
#include "darcel/semantics/element.hpp"
#include "darcel/semantics/function.hpp"
#include "darcel/semantics/semantics.hpp"
#include "darcel/semantics/variable.hpp"
#include "darcel/syntax/ops.hpp"

namespace darcel {

  //! Scopes are used to store and look-up named language constructs such as
  //! variables, data types, functions, etc...
  class scope {
    public:

      //! Constructs an empty global scope.
      scope();

      //! Constructs a child scope.
      /*!
        \param parent The parent scope.
      */
      scope(const scope* parent);

      //! Tests if an element with a specified name is contained within this
      //! scope (that is directly within this scope, not a parent scope).
      bool contains(const std::string& name) const;

      //! Finds an element accessible from within this scope (ie. contained
      //! within this scope or a parent scope).
      std::shared_ptr<element> find(const std::string& name) const;

      //! Finds an element of a particular type within this scope.
      template<typename T>
      std::shared_ptr<T> find(const std::string& name) const;

      //! Finds an element within this scope (ie. contained strictly within this
      //! scope, not a parent scope).
      std::shared_ptr<element> find_within(const std::string& name) const;

      //! Adds an element to this scope.
      /*!
        \param e The element to add.
        \return <code>true</code> iff the element was added, otherwise an
                element with the same name already exists.
      */
      bool add(std::shared_ptr<element> e);

    private:
      const scope* m_parent;
      std::unordered_map<std::string, std::shared_ptr<element>> m_elements;

      scope(const scope&) = delete;
      scope& operator =(const scope&) = delete;
  };

  //! Populates a scope with all global/built-in elements.
  /*!
    \param scope The scope to populate.
  */
  inline void populate_global_scope(scope& scope) {
    scope.add(bool_data_type::get_instance());
    scope.add(float_data_type::get_instance());
    scope.add(integer_data_type::get_instance());
    scope.add(text_data_type::get_instance());
    for(auto& t : std::vector<std::shared_ptr<data_type>>{
        integer_data_type::get_instance(), float_data_type::get_instance()}) {
      std::vector<function_data_type::parameter> parameters;
      parameters.push_back({"lhs", t});
      parameters.push_back({"rhs", t});
      for(auto& o : {op::ADD, op::SUBTRACT, op::MULTIPLY, op::DIVIDE}) {
        auto f = std::make_shared<variable>(location::global(),
          get_function_name(o),
          std::make_shared<function_data_type>(parameters, t));
        scope.add(f);
      }
    }
    for(auto& t : std::vector<std::shared_ptr<data_type>>{
        bool_data_type::get_instance(), integer_data_type::get_instance(),
        float_data_type::get_instance(), text_data_type::get_instance()}) {
      std::vector<function_data_type::parameter> parameters;
      parameters.push_back({"v", t});
      auto f = std::make_shared<variable>(location::global(), "print",
        std::make_shared<function_data_type>(parameters, t));
      scope.add(f);
    }
    for(auto& t : std::vector<std::shared_ptr<data_type>>{
        bool_data_type::get_instance(), integer_data_type::get_instance(),
        float_data_type::get_instance(), text_data_type::get_instance()}) {
      std::vector<function_data_type::parameter> parameters;
      parameters.push_back({"initial", t});
      parameters.push_back({"continuation", t});
      auto f = std::make_shared<variable>(location::global(), "chain",
        std::make_shared<function_data_type>(parameters, t));
      scope.add(f);
    }
    {
      std::vector<function_data_type::parameter> parameters;
      parameters.push_back({"lhs", text_data_type::get_instance()});
      parameters.push_back({"rhs", text_data_type::get_instance()});
      auto name = get_function_name(op::ADD);
      auto f = std::make_shared<variable>(location::global(), name,
        std::make_shared<function_data_type>(parameters,
        text_data_type::get_instance()));
      scope.add(f);
    }
  }

  inline scope::scope()
      : m_parent(nullptr) {}

  inline scope::scope(const scope* parent)
      : m_parent(parent) {}

  inline bool scope::contains(const std::string& name) const {
    return m_elements.count(name) == 1;
  }

  inline std::shared_ptr<element> scope::find(const std::string& name) const {
    auto e = m_elements.find(name);
    if(e == m_elements.end()) {
      if(m_parent == nullptr) {
        return nullptr;
      }
      return m_parent->find(name);
    }
    return e->second;
  }

  template<typename T>
  std::shared_ptr<T> scope::find(const std::string& name) const {
    auto e = find(name);
    return std::dynamic_pointer_cast<T>(e);
  }

  inline std::shared_ptr<element> scope::find_within(
      const std::string& name) const {
    auto e = m_elements.find(name);
    if(e == m_elements.end()) {
      return nullptr;
    }
    return e->second;
  }

  inline bool scope::add(std::shared_ptr<element> e) {
    if(auto v = std::dynamic_pointer_cast<variable>(e)) {
      if(std::dynamic_pointer_cast<function_data_type>(v->get_data_type())) {
        auto f = m_elements.find(e->get_name());
        if(f == m_elements.end()) {
          auto g = std::make_shared<function>(std::move(v));
          return m_elements.insert(std::make_pair(g->get_name(), g)).second;
        } else if(auto g = std::dynamic_pointer_cast<function>(f->second)) {
          g->add(std::move(v));
          return true;
        } else {
          return false;
        }
      }
    }
    return m_elements.try_emplace(e->get_name(), e).second;
  }
}

#endif
