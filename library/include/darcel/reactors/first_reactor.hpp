#ifndef DARCEL_FIRST_REACTOR_HPP
#define DARCEL_FIRST_REACTOR_HPP
#include <memory>
#include <utility>
#include "darcel/reactors/constant_reactor.hpp"
#include "darcel/reactors/function_reactor.hpp"
#include "darcel/reactors/reactors.hpp"

namespace darcel {

  //! Makes a reactor that evaluates to the first value it receives from its
  //! source.
  /*!
    \param source The source that will provide the value to evaluate to.
  */
  template<typename T>
  auto make_first_reactor(std::shared_ptr<reactor<T>> source) {
    return make_function_reactor(
      [] (auto&& value) {
        return make_function_evaluation(std::forward<decltype(value)>(value),
          base_reactor::update::COMPLETE);
      }, std::move(source));
  }

  //! Makes a reactor that evaluates to the first value it receives from its
  //! source.
  /*!
    \param source The source that will provide the value to evaluate to.
  */
  template<typename R>
  auto first(R&& source) {
    return make_first_reactor(lift(std::forward<R>(source)));
  }

  //! Builds a first reactor.
  template<typename T>
  auto make_first_reactor_builder() {
    return std::make_unique<function_reactor_builder>(
      [] (auto& parameters, auto& t) {
        return make_first_reactor(
          std::static_pointer_cast<reactor<T>>(parameters.front()->build(t)));
      });
  }
}

#endif
