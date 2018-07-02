#ifndef DARCEL_FUNCTION_HPP
#define DARCEL_FUNCTION_HPP
#include "darcel/semantics/element.hpp"
#include "darcel/semantics/semantics.hpp"

namespace darcel {

  //! Represents a potentially overloaded, named function.
  class function final : public element {
    public:

      //! Constructs a function.
      /*!
        \param initial The initial overload.
      */
      function(location l, std::string name);

      const location& get_location() const override;

      const std::string& get_name() const override;
  };
}

#endif
