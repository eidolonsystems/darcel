#ifndef DARCEL_VARIABLE_HPP
#define DARCEL_VARIABLE_HPP
#include "darcel/data_types/data_type.hpp"
#include "darcel/semantics/element.hpp"
#include "darcel/semantics/semantics.hpp"

namespace darcel {

  //! Represents a variable.
  class variable final : public element {
    public:

      //! Constructs a variable.
      /*!
        \param l The location where the variable was declared.
        \param name The name of the variable.
        \param d The variable's data type.
      */
      variable(location l, std::string name, std::shared_ptr<data_type> d);

      //! Returns the variable's data type.
      const std::shared_ptr<data_type>& get_data_type() const;

      const location& get_location() const override;

      const std::string& get_name() const override;

    private:
      location m_location;
      std::string m_name;
      std::shared_ptr<data_type> m_data_type;
  };

  inline variable::variable(location l, std::string name,
      std::shared_ptr<data_type> d)
      : m_location(std::move(l)),
        m_name(std::move(name)),
        m_data_type(std::move(d)) {}

  inline const std::shared_ptr<data_type>& variable::get_data_type() const {
    return m_data_type;
  }

  inline const location& variable::get_location() const {
    return m_location;
  }

  inline const std::string& variable::get_name() const {
    return m_name;
  }
}

#endif
