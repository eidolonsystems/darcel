#ifndef DARCEL_FUNCTION_HPP
#define DARCEL_FUNCTION_HPP
#include "darcel/semantics/element.hpp"
#include "darcel/semantics/semantics.hpp"

namespace darcel {

  //! Represents a potentially overloaded function declaration.
  class function final : public element {
    public:

      //! Constructs a root function.
      /*!
        \param l The location where the function was first declared.
        \param name The name of the function.
      */
      function(Location l, std::string name);

      //! Constructs a root function.
      /*!
        \param name The name of the function.
      */
      explicit function(std::string name);

      //! Constructs a child function.
      /*!
        \param l The location where the function was first declared.
        \param parent The parent function.
      */
      function(Location l, std::shared_ptr<function> parent);

      //! Constructs a child function.
      /*!
        \param parent The parent function.
      */
      explicit function(std::shared_ptr<function> parent);

      //! Returns the parent function, or null if this function is the root.
      const std::shared_ptr<function>& get_parent() const;

      const Location& get_location() const override;

      const std::string& get_name() const override;

    private:
      Location m_location;
      std::string m_name;
      std::shared_ptr<function> m_parent;
  };

  inline function::function(Location l, std::string name)
      : m_location(std::move(l)),
        m_name(std::move(name)) {}

  inline function::function(std::string name)
      : function(Location::global(), std::move(name)) {}

  inline function::function(Location l, std::shared_ptr<function> parent)
      : m_location(std::move(l)),
        m_name(parent->get_name()),
        m_parent(std::move(parent)) {}

  inline function::function(std::shared_ptr<function> parent)
      : function(Location::global(), std::move(parent)) {}

  inline const std::shared_ptr<function>& function::get_parent() const {
    return m_parent;
  }

  inline const Location& function::get_location() const {
    return m_location;
  }

  inline const std::string& function::get_name() const {
    return m_name;
  }
}

#endif
