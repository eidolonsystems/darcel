#ifndef DARCEL_NONE_REACTOR_HPP
#define DARCEL_NONE_REACTOR_HPP
#include <memory>
#include "darcel/reactors/reactor.hpp"
#include "darcel/reactors/reactor_unavailable_exception.hpp"
#include "darcel/reactors/reactors.hpp"

namespace darcel {

  //! A reactor that never produces an evaluation.
  template<typename T>
  class none_reactor : public reactor<T> {
    public:
      using type = typename reactor<T>::type;

      //! Constructs a none reactor.
      none_reactor() = default;

      base_reactor::update commit(int sequence) override final;

      type eval() const override final;
  };

  //! Makes a none reactor.
  template<typename T>
  auto make_none_reactor() {
    return std::make_shared<none_reactor<T>>();
  };

  //! Makes a none reactor.
  template<typename T>
  auto none() {
    return make_none_reactor<T>();
  }

  template<typename T>
  base_reactor::update none_reactor<T>::commit(int sequence) {
    return base_reactor::update::COMPLETE_EMPTY;
  }

  template<typename T>
  typename none_reactor<T>::type none_reactor<T>::eval() const {
    throw reactor_unavailable_exception();
  }
}

#endif
