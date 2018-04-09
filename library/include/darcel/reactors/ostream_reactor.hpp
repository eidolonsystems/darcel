#ifndef DARCEL_OSTREAM_REACTOR_HPP
#define DARCEL_OSTREAM_REACTOR_HPP
#include <memory>
#include <ostream>
#include "darcel/reactors/reactor.hpp"
#include "darcel/reactors/reactor_builder.hpp"
#include "darcel/reactors/reactors.hpp"

namespace darcel {

  //! Prints the values produced by its parameter.
  /*!
    \param out The stream to print to.
    \param source The reactor producing the values to print.
  */
  template<typename T>
  auto make_ostream_reactor(std::ostream& out,
      std::shared_ptr<reactor<T>> source) {
    return make_function_reactor(
      [out = &out] (const T& value) -> decltype(auto) {
        *out << value << "\n";
        return value;
      }, std::move(source));
  }

  //! Prints boolean values produced by its parameter.
  /*!
    \param out The stream to print to.
    \param source The reactor producing the values to print.
  */
  inline auto make_ostream_reactor(std::ostream& out,
      std::shared_ptr<reactor<bool>> source) {
    return make_function_reactor(
      [out = &out] (bool value) {
        if(value) {
          *out << "true\n";
        } else {
          *out << "false\n";
        }
        return value;
      }, std::move(source));
  }

  //! Builds an ostream reactor.
  template<typename T>
  auto make_ostream_reactor_builder(std::ostream& out) {
    return std::make_unique<function_reactor_builder>(
      [out = &out] (auto& parameters, auto& t) {
        return make_ostream_reactor(*out,
          std::static_pointer_cast<reactor<T>>(parameters.front()->build(t)));
      });
  }
}

#endif
