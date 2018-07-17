#ifndef DARCEL_SCOPE_HPP
#define DARCEL_SCOPE_HPP
#include <memory>
#include <unordered_map>
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

      //! Constructs a child scope.
      /*!
        \param parent The parent scope.
      */
      explicit scope(const scope* parent);

      //! Returns the parent scope (potentially null).
      const scope* get_parent() const;

      //! Tests if an element with a specified name is contained within this
      //! scope (that is directly within this scope, not a parent scope).
      bool contains(const std::string& name) const;

      //! Finds an element accessible from within this scope (ie. contained
      //! within this scope or a parent scope).
      template<typename F>
      std::shared_ptr<element> find(F&& predicate) const;

      //! Finds an element accessible from within this scope (ie. contained
      //! within this scope or a parent scope).
      std::shared_ptr<element> find(const std::string& name) const;

      //! Finds an element of a particular type within this scope.
      template<typename T>
      std::shared_ptr<T> find(const std::string& name) const;

      //! Finds an element within this scope (ie. contained strictly within this
      //! scope, not a parent scope).
      std::shared_ptr<element> find_within(const std::string& name) const;

      //! Returns a function's definitions contained strictly within this scope.
      const std::vector<std::shared_ptr<function_definition>>&
        get_definitions(const function& f) const;

      //! Finds a definition matching a predicate.
      /*!
        \param f The function whose definitions are searched over.
        \param predicate The predicate to match.
        \return The function definition matching the predicate.
      */
      template<typename F>
      const std::shared_ptr<function_definition>& find(const function& f,
        F&& predicate) const;

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
      std::unordered_map<const function*,
        std::vector<std::shared_ptr<function_definition>>>
        m_function_definitions;

      scope(const scope&) = delete;
      scope& operator =(const scope&) = delete;
  };

  inline scope::scope()
      : scope(nullptr) {}

  inline scope::scope(const scope* parent)
      : m_parent(parent) {}

  inline const scope* scope::get_parent() const {
    return m_parent;
  }

  inline bool scope::contains(const std::string& name) const {
    return m_elements.count(name) == 1;
  }

  template<typename F>
  std::shared_ptr<element> scope::find(F&& predicate) const {
    for(auto& element : m_elements) {
      if(predicate(element.second)) {
        return element.second;
      }
    }
    for(auto& f : m_function_definitions) {
      for(auto& definition : f.second) {
        if(predicate(definition)) {
          return definition;
        }
      }
    }
    if(m_parent == nullptr) {
      static const std::shared_ptr<element> NONE;
      return NONE;
    }
    return m_parent->find(std::move(predicate));
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

  inline const std::vector<std::shared_ptr<function_definition>>&
      scope::get_definitions(const function& f) const {
    static std::vector<std::shared_ptr<function_definition>> EMPTY;
    auto i = m_function_definitions.find(&f);
    if(i == m_function_definitions.end()) {
      return EMPTY;
    }
    return i->second;
  }

  template<typename F>
  const std::shared_ptr<function_definition>& scope::find(const function& f,
      F&& predicate) const {
    for(auto& definition : get_definitions(f)) {
      if(predicate(*definition)) {
        return definition;
      }
    }
    if(m_parent == nullptr) {
      static const std::shared_ptr<function_definition> NONE;
      return NONE;
    }
    return m_parent->find(f, std::move(predicate));
  }

  inline bool scope::add(std::shared_ptr<element> e) {
    if(auto definition = std::dynamic_pointer_cast<function_definition>(e)) {
      auto f = find<function>(e->get_name());
      if(f == nullptr) {
        return false;
      }
      auto& definitions = m_function_definitions[f.get()];
      for(auto& d : definitions) {
        if(*d->get_type() == *definition->get_type()) {
          return false;
        }
      }
      definitions.push_back(std::move(definition));
      return true;
    } else {
      return m_elements.try_emplace(e->get_name(), e).second;
    }
  }
}

#endif
