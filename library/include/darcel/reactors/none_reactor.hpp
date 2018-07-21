#ifndef DARCEL_NONE_REACTOR_HPP
#define DARCEL_NONE_REACTOR_HPP
#include <memory>
#include "darcel/reactors/reactor.hpp"
#include "darcel/reactors/reactor_unavailable_exception.hpp"
#include "darcel/reactors/reactors.hpp"

namespace darcel {

  //! A reactor that never produces an evaluation.
  template<typename T>
  class NoneReactor final : public Reactor<T> {
    public:
      using Type = typename Reactor<T>::Type;

      //! Constructs a none reactor.
      NoneReactor() = default;

      BaseReactor::Update commit(int sequence) override;

      Type eval() const override;
  };

  //! Makes a none reactor.
  template<typename T>
  auto make_none_reactor() {
    return std::make_shared<NoneReactor<T>>();
  };

  //! Makes a none reactor.
  template<typename T>
  auto none() {
    return make_none_reactor<T>();
  }

  template<typename T>
  BaseReactor::Update NoneReactor<T>::commit(int sequence) {
    return BaseReactor::Update::COMPLETE_EMPTY;
  }

  template<typename T>
  typename NoneReactor<T>::Type NoneReactor<T>::eval() const {
    throw ReactorUnavailableException();
  }
}

#endif
