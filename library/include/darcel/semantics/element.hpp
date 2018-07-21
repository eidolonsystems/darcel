#ifndef DARCEL_ELEMENT_HPP
#define DARCEL_ELEMENT_HPP
#include <memory>
#include <string>
#include "darcel/lexicon/location.hpp"
#include "darcel/semantics/semantics.hpp"

namespace darcel {

  //! Base class for a named element of the language that can be placed in a
  //! scope.
  class Element {
    public:

      //! Constructs an element.
      Element() = default;

      virtual ~Element() = default;

      //! Returns the location where the element was declared.
      virtual const Location& get_location() const = 0;

      //! Returns the name of the element.
      virtual const std::string& get_name() const = 0;

    private:
      Element(const Element&) = delete;
      Element& operator =(const Element&) = delete;
  };
}

#endif
