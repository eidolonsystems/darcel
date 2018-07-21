#ifndef DARCEL_OPERATORS_HPP
#define DARCEL_OPERATORS_HPP
#include <memory>
#include <utility>
#include "darcel/reactors/function_reactor.hpp"
#include "darcel/reactors/reactor_builder.hpp"
#include "darcel/reactors/reactors.hpp"

namespace darcel {

  //! Makes a reactor that adds two reactors.
  /*!
    \param lhs The left hand side.
    \param rhs The right hand side.
  */
  template<typename T, typename U>
  auto add(std::shared_ptr<Reactor<T>> lhs, std::shared_ptr<Reactor<U>> rhs) {
    return make_function_reactor(
      [] (const T& lhs, const U& rhs) {
        return lhs + rhs;
      }, std::move(lhs), std::move(rhs));
  }

  //! Builds a reactor that adds two sub-reactors.
  template<typename T, typename U = T>
  auto make_add_reactor_builder() {
    return std::make_unique<FunctionReactorBuilder>(
      [] (auto& parameters, auto& t) {
        return add(
          std::static_pointer_cast<Reactor<T>>(parameters[0]->build(t)),
          std::static_pointer_cast<Reactor<U>>(parameters[1]->build(t)));
      });
  };

  //! Makes a reactor that divides two reactors.
  /*!
    \param lhs The left hand side.
    \param rhs The right hand side.
  */
  template<typename T, typename U>
  auto divide(std::shared_ptr<Reactor<T>> lhs,
      std::shared_ptr<Reactor<U>> rhs) {
    return make_function_reactor(
      [] (const T& lhs, const U& rhs) {
        return lhs / rhs;
      }, std::move(lhs), std::move(rhs));
  }

  //! Builds a reactor that divides two sub-reactors.
  template<typename T, typename U = T>
  auto make_divide_reactor_builder() {
    return std::make_unique<FunctionReactorBuilder>(
      [] (auto& parameters, auto& t) {
        return divide(
          std::static_pointer_cast<Reactor<T>>(parameters[0]->build(t)),
          std::static_pointer_cast<Reactor<U>>(parameters[1]->build(t)));
      });
  };

  //! Makes a reactor that multiplies two reactors.
  /*!
    \param lhs The left hand side.
    \param rhs The right hand side.
  */
  template<typename T, typename U>
  auto multiply(std::shared_ptr<Reactor<T>> lhs,
      std::shared_ptr<Reactor<U>> rhs) {
    return make_function_reactor(
      [] (const T& lhs, const U& rhs) {
        return lhs * rhs;
      }, std::move(lhs), std::move(rhs));
  }

  //! Builds a reactor that multiplies two sub-reactors.
  template<typename T, typename U = T>
  auto make_multiply_reactor_builder() {
    return std::make_unique<FunctionReactorBuilder>(
      [] (auto& parameters, auto& t) {
        return multiply(
          std::static_pointer_cast<Reactor<T>>(parameters[0]->build(t)),
          std::static_pointer_cast<Reactor<U>>(parameters[1]->build(t)));
      });
  };

  //! Makes a reactor that subtracts two reactors.
  /*!
    \param lhs The left hand side.
    \param rhs The right hand side.
  */
  template<typename T, typename U>
  auto subtract(std::shared_ptr<Reactor<T>> lhs,
      std::shared_ptr<Reactor<U>> rhs) {
    return make_function_reactor(
      [] (const T& lhs, const U& rhs) {
        return lhs - rhs;
      }, std::move(lhs), std::move(rhs));
  }

  //! Builds a reactor that subtracts two sub-reactors.
  template<typename T, typename U = T>
  auto make_subtract_reactor_builder() {
    return std::make_unique<FunctionReactorBuilder>(
      [] (auto& parameters, auto& t) {
        return subtract(
          std::static_pointer_cast<Reactor<T>>(parameters[0]->build(t)),
          std::static_pointer_cast<Reactor<U>>(parameters[1]->build(t)));
      });
  };
}

#endif
