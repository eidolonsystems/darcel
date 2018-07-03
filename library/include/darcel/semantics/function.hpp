#ifndef DARCEL_FUNCTION_HPP
#define DARCEL_FUNCTION_HPP
#include "darcel/semantics/element.hpp"
#include "darcel/semantics/semantics.hpp"

namespace darcel {

  //! Represents a potentially overloaded function declaration.
  class function final : public element {
    public:

      //! Constructs a function.
      /*!
        \param l The location where the function was first declared.
        \param name The name of the function.
      */
      function(location l, std::string name);

      const location& get_location() const override;

      const std::string& get_name() const override;

    private:
      location m_location;
      std::string m_name;
  };

  inline function::function(location l, std::string name)
      : m_location(std::move(l)),
        m_name(std::move(name)) {}

  inline const location& function::get_location() const {
    return m_location;
  }

  inline const std::string& function::get_name() const {
    return m_name;
  }
}

#endif
