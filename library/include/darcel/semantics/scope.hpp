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
  class Scope {
    public:

      //! Constructs an empty global scope.
      Scope();

      //! Constructs a child scope.
      /*!
        \param parent The parent scope.
      */
      explicit Scope(const Scope* parent);

      //! Returns the parent scope (potentially null).
      const Scope* get_parent() const;

      //! Tests if an element with a specified name is contained within this
      //! scope (that is directly within this scope, not a parent scope).
      bool contains(const std::string& name) const;

      //! Finds an element accessible from within this scope (ie. contained
      //! within this scope or a parent scope).
      template<typename F>
      std::shared_ptr<Element> find(F&& predicate) const;

      //! Finds an element accessible from within this scope (ie. contained
      //! within this scope or a parent scope).
      std::shared_ptr<Element> find(const std::string& name) const;

      //! Finds an element of a particular type within this scope.
      template<typename T>
      std::shared_ptr<T> find(const std::string& name) const;

      //! Finds an element within this scope (ie. contained strictly within this
      //! scope, not a parent scope).
      std::shared_ptr<Element> find_within(const std::string& name) const;

      //! Returns a function's definitions contained strictly within this scope.
      const std::vector<std::shared_ptr<FunctionDefinition>>&
        get_definitions(const Function& f) const;

      //! Finds a definition matching a predicate.
      /*!
        \param f The function whose definitions are searched over.
        \param predicate The predicate to match.
        \return The function definition matching the predicate.
      */
      template<typename F>
      const std::shared_ptr<FunctionDefinition>& find(const Function& f,
        F&& predicate) const;

      //! Adds an element to this scope.
      /*!
        \param e The element to add.
        \return <code>true</code> iff the element was added, otherwise an
                element with the same name already exists.
      */
      bool add(std::shared_ptr<Element> e);

    private:
      const Scope* m_parent;
      std::unordered_map<std::string, std::shared_ptr<Element>> m_elements;
      std::unordered_map<const Function*,
        std::vector<std::shared_ptr<FunctionDefinition>>>
        m_function_definitions;

      Scope(const Scope&) = delete;
      Scope& operator =(const Scope&) = delete;
  };

  inline Scope::Scope()
      : Scope(nullptr) {}

  inline Scope::Scope(const Scope* parent)
      : m_parent(parent) {}

  inline const Scope* Scope::get_parent() const {
    return m_parent;
  }

  inline bool Scope::contains(const std::string& name) const {
    return m_elements.count(name) == 1;
  }

  template<typename F>
  std::shared_ptr<Element> Scope::find(F&& predicate) const {
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
      static const std::shared_ptr<Element> NONE;
      return NONE;
    }
    return m_parent->find(std::move(predicate));
  }

  inline std::shared_ptr<Element> Scope::find(const std::string& name) const {
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
  std::shared_ptr<T> Scope::find(const std::string& name) const {
    auto e = find(name);
    return std::dynamic_pointer_cast<T>(e);
  }

  inline std::shared_ptr<Element> Scope::find_within(
      const std::string& name) const {
    auto e = m_elements.find(name);
    if(e == m_elements.end()) {
      return nullptr;
    }
    return e->second;
  }

  inline const std::vector<std::shared_ptr<FunctionDefinition>>&
      Scope::get_definitions(const Function& f) const {
    static std::vector<std::shared_ptr<FunctionDefinition>> EMPTY;
    auto i = m_function_definitions.find(&f);
    if(i == m_function_definitions.end()) {
      return EMPTY;
    }
    return i->second;
  }

  template<typename F>
  const std::shared_ptr<FunctionDefinition>& Scope::find(const Function& f,
      F&& predicate) const {
    for(auto& definition : get_definitions(f)) {
      if(predicate(*definition)) {
        return definition;
      }
    }
    if(m_parent == nullptr) {
      static const std::shared_ptr<FunctionDefinition> NONE;
      return NONE;
    }
    return m_parent->find(f, std::move(predicate));
  }

  inline bool Scope::add(std::shared_ptr<Element> e) {
    if(auto definition = std::dynamic_pointer_cast<FunctionDefinition>(e)) {
      auto f = find<Function>(e->get_name());
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
