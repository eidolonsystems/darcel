#ifndef DARCEL_ENUM_TO_STRING_REACTOR_HPP
#define DARCEL_ENUM_TO_STRING_REACTOR_HPP
#include <memory>
#include "darcel/data_types/enum_data_type.hpp"
#include "darcel/reactors/function_reactor.hpp"
#include "darcel/reactors/reactor.hpp"
#include "darcel/reactors/reactor_builder.hpp"
#include "darcel/reactors/reactors.hpp"

namespace darcel {

  //! Converts an enum to a string.
  /*!
    \param e The enum type.
    \param source The reactor producing the values to convert.
  */
  inline auto make_enum_to_string_reactor(std::shared_ptr<enum_data_type> e,
      std::shared_ptr<reactor<int>> source) {
    return make_function_reactor(
      [=] (auto value) {
        for(auto& symbol : e->get_symbols()) {
          if(symbol.m_value == value) {
            return e->get_name() + "." + symbol.m_name;
          }
        }
        return std::string();
      }, std::move(source));
  }

  //! Builds an enum to string reactor.
  /*!
    \param e The enum type.
  */
  inline auto make_enum_to_string_reactor_builder(
      std::shared_ptr<enum_data_type> e) {
    return std::make_unique<function_reactor_builder>(
      [=] (auto& parameters, auto& t) {
        return make_enum_to_string_reactor(std::move(e),
          std::static_pointer_cast<reactor<int>>(parameters.front()->build(t)));
      });
  }
}

#endif
