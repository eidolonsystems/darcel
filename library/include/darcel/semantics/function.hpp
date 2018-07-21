#ifndef DARCEL_FUNCTION_HPP
#define DARCEL_FUNCTION_HPP
#include "darcel/semantics/element.hpp"
#include "darcel/semantics/semantics.hpp"

namespace darcel {

  //! Represents a potentially overloaded function declaration.
  class Function final : public Element {
    public:

      //! Constructs a root function.
      /*!
        \param l The location where the function was first declared.
        \param name The name of the function.
      */
      Function(Location l, std::string name);

      //! Constructs a root function.
      /*!
        \param name The name of the function.
      */
      explicit Function(std::string name);

      //! Constructs a child function.
      /*!
        \param l The location where the function was first declared.
        \param parent The parent function.
      */
      Function(Location l, std::shared_ptr<Function> parent);

      //! Constructs a child function.
      /*!
        \param parent The parent function.
      */
      explicit Function(std::shared_ptr<Function> parent);

      //! Returns the parent function, or null if this function is the root.
      const std::shared_ptr<Function>& get_parent() const;

      const Location& get_location() const override;

      const std::string& get_name() const override;

    private:
      Location m_location;
      std::string m_name;
      std::shared_ptr<Function> m_parent;
  };

  inline Function::Function(Location l, std::string name)
      : m_location(std::move(l)),
        m_name(std::move(name)) {}

  inline Function::Function(std::string name)
      : Function(Location::global(), std::move(name)) {}

  inline Function::Function(Location l, std::shared_ptr<Function> parent)
      : m_location(std::move(l)),
        m_name(parent->get_name()),
        m_parent(std::move(parent)) {}

  inline Function::Function(std::shared_ptr<Function> parent)
      : Function(Location::global(), std::move(parent)) {}

  inline const std::shared_ptr<Function>& Function::get_parent() const {
    return m_parent;
  }

  inline const Location& Function::get_location() const {
    return m_location;
  }

  inline const std::string& Function::get_name() const {
    return m_name;
  }
}

#endif
