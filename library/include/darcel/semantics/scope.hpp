#ifndef DARCEL_SCOPE_HPP
#define DARCEL_SCOPE_HPP
#include <memory>
#include <unordered_map>
#include <vector>
#include "darcel/semantics/element.hpp"
#include "darcel/semantics/function.hpp"
#include "darcel/semantics/function_definition.hpp"
#include "darcel/semantics/semantics.hpp"
#include "darcel/semantics/variable.hpp"

namespace darcel {

  //! Scopes are used to store and look-up named language constructs such as
  //! variables, data types, functions, etc...
  class scope {
    public:

      //! Constructs an empty global scope.
      scope();

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

      //! Makes an empty child scope directly owned by this scope.
      scope& make_child();

    private:
      const scope* m_parent;
      std::vector<std::unique_ptr<scope>> m_children;
      std::unordered_map<std::string, std::shared_ptr<element>> m_elements;
      std::unordered_map<std::string,
        std::vector<std::shared_ptr<function_definition>>>
        m_function_definitions;

      scope(const scope* parent);
      scope(const scope&) = delete;
      scope& operator =(const scope&) = delete;
  };

  inline scope::scope()
      : m_parent(nullptr) {}

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
    if(auto f = std::dynamic_pointer_cast<function_definition>(e)) {
      auto& definitions = m_function_definitions[e->get_name()];
      for(auto& definition : definitions) {
        if(*definition->get_type() == *f->get_type()) {
          return false;
        }
      }
      definitions.push_back(std::move(f));
      return true;
    } else {
      return m_elements.try_emplace(e->get_name(), e).second;
    }
  }

  inline scope& scope::make_child() {
    m_children.push_back(std::unique_ptr<scope>(new scope(this)));
    return *m_children.back();
  }

  inline scope::scope(const scope* parent)
      : m_parent(parent) {}

}

#endif
