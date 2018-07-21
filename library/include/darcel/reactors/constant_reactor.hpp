#ifndef DARCEL_CONSTANT_REACTOR_HPP
#define DARCEL_CONSTANT_REACTOR_HPP
#include <memory>
#include <type_traits>
#include <utility>
#include "darcel/reactors/reactor.hpp"
#include "darcel/reactors/reactor_builder.hpp"
#include "darcel/reactors/reactors.hpp"

namespace darcel {

  //! Evaluates to a constant.
  template<typename T>
  class ConstantReactor final : public Reactor<T> {
    public:
      using Type = typename Reactor<T>::Type;

      //! Constructs a constant.
      /*!
        \param value The constant to evaluate to.
      */
      template<typename V>
      ConstantReactor(V&& value);

      BaseReactor::Update commit(int sequence) override;

      Type eval() const override;

    private:
      Type m_value;
  };

  //! Makes a constant.
  /*!
    \param value The constant to evaluate to.
  */
  template<typename T>
  auto make_constant_reactor(T&& value) {
    return std::make_shared<ConstantReactor<std::decay_t<T>>>(
      std::forward<T>(value));
  }

  //! Makes a constant reactor builder.
  /*!
    \param value The constant to evaluate to.
  */
  template<typename T>
  auto make_constant_reactor_builder(T&& value) {
    return std::make_unique<FunctionReactorBuilder>(
      [value = std::forward<T>(value)] (auto& parameters, auto& t) {
        return make_constant_reactor(value);
      });
  }

  template<typename T>
  template<typename V>
  ConstantReactor<T>::ConstantReactor(V&& value)
      : m_value(std::forward<V>(value)) {}

  template<typename T>
  BaseReactor::Update ConstantReactor<T>::commit(int sequence) {
    return BaseReactor::Update::COMPLETE_EVAL;
  }

  template<typename T>
  typename ConstantReactor<T>::Type ConstantReactor<T>::eval() const {
    return m_value;
  }

namespace details {
  template<typename T>
  struct lift_helper {
    template<typename U>
    auto operator ()(U&& value) const {
      return std::static_pointer_cast<Reactor<T>>(
        make_constant_reactor(std::forward<U>(value)));
    }
  };

  template<typename T>
  struct lift_helper<std::shared_ptr<T>> {
    template<typename U>
    auto operator ()(U&& value) const {
      return std::static_pointer_cast<Reactor<reactor_type_t<T>>>(
        std::forward<U>(value));
    }
  };
}

  //! Lifts a constant value to a reactor unless the parameter is already a
  //! reactor type.
  /*!
    \param value The value to lift.
  */
  template<typename T>
  decltype(auto) lift(T&& value) {
    return details::lift_helper<std::decay_t<T>>()(std::forward<T>(value));
  }
}

#endif
