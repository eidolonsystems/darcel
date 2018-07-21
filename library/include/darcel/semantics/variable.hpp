#ifndef DARCEL_VARIABLE_HPP
#define DARCEL_VARIABLE_HPP
#include "darcel/semantics/element.hpp"
#include "darcel/semantics/semantics.hpp"

namespace darcel {

  //! Represents a variable.
  class Variable final : public Element {
    public:

      //! Constructs a variable.
      /*!
        \param l The location where the variable was declared.
        \param name The name of the variable.
      */
      Variable(Location l, std::string name);

      const Location& get_location() const override;

      const std::string& get_name() const override;

    private:
      Location m_location;
      std::string m_name;
  };

  inline Variable::Variable(Location l, std::string name)
      : m_location(std::move(l)),
        m_name(std::move(name)) {}

  inline const Location& Variable::get_location() const {
    return m_location;
  }

  inline const std::string& Variable::get_name() const {
    return m_name;
  }
}

#endif
