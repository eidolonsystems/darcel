#ifndef DARCEL_INVALID_MEMBER_SYNTAX_ERROR_HPP
#define DARCEL_INVALID_MEMBER_SYNTAX_ERROR_HPP
#include <memory>
#include <string>
#include <utility>
#include "darcel/semantics/element.hpp"
#include "darcel/syntax/syntax.hpp"
#include "darcel/syntax/syntax_error.hpp"

namespace darcel {

  //! Indicates an invalid member is being accessed.
  class invalid_member_syntax_error : public syntax_error {
    public:

      //! Constructs an invalid member syntax erorr.
      /*!
        \param error_location The location of the operation.
        \param e The element being accessed.
        \param name The name of the invalid member.
      */
      invalid_member_syntax_error(Location error_location,
        std::shared_ptr<Element> e, std::string name);

      //! Returns the element being accessed.
      const std::shared_ptr<Element>& get_element() const;

      //! Returns the name of the invalid member.
      const std::string& get_name() const;

    private:
      std::shared_ptr<Element> m_element;
      std::string m_name;
  };

  inline invalid_member_syntax_error::invalid_member_syntax_error(
      Location error_location, std::shared_ptr<Element> e, std::string name)
      : syntax_error(syntax_error_code::INVALID_MEMBER,
          std::move(error_location)),
        m_element(std::move(e)),
        m_name(std::move(name)) {}

  inline const std::shared_ptr<Element>&
      invalid_member_syntax_error::get_element() const {
    return m_element;
  }

  inline const std::string& invalid_member_syntax_error::get_name() const {
    return m_name;
  }
}

#endif
