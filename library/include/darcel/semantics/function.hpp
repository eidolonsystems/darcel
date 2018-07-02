#ifndef DARCEL_FUNCTION_HPP
#define DARCEL_FUNCTION_HPP
#include "darcel/semantics/element.hpp"
#include "darcel/semantics/semantics.hpp"

namespace darcel {

  //! Represents a named function.
  class function final : public element {
    public:

      //! Constructs a top-level function.
      /*!
        \param l The location where the function was declared.
        \param name The name of the function.
      */
      function(location l, std::string name);

      //! Constructs a function sharing the same name as a function in the
      //! parent scope.
      /*!
        \param l The location where the function was declared.
        \param f The parent function.
      */
      function(location l, const function& parent);

      //! Returns the parent function.
      const function* get_parent() const;

      const location& get_location() const override;

      const std::string& get_name() const override;

    private:
      location m_location;
      std::string m_name;
      const function* m_parent;
  };

  inline function::function(location l, std::string name)
      : m_location(std::move(l)),
        m_name(std::move(name)),
        m_parent(nullptr) {}

  inline function::function(location l, const function& parent)
      : m_location(std::move(l)),
        m_name(parent.get_name()),
        m_parent(&parent) {}

  inline const function* function::get_parent() const {
    return m_parent;
  }

  inline const location& function::get_location() const {
    return m_location;
  }

  inline const std::string& function::get_name() const {
    return m_name;
  }
}

#endif
