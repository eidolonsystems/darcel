#ifndef DARCEL_CONSTANT_REACTOR_HPP
#define DARCEL_CONSTANT_REACTOR_HPP
#include <memory>
#include <type_traits>
#include <utility>
#include "darcel/reactors/reactor.hpp"
#include "darcel/reactors/reactors.hpp"

namespace darcel {

  //! Evaluates to a constant.
  template<typename T>
  class constant_reactor : public reactor<T> {
    public:
      using type = typename reactor<T>::type;

      //! Constructs a constant.
      /*!
        \param value The constant to evaluate to.
      */
      template<typename V>
      constant_reactor(V&& value);

      base_reactor::update commit(int sequence) override final;

      type eval() const override final;

    private:
      int m_sequence;
      type m_value;
  };

  //! Makes a constant.
  /*!
    \param value The constant to evaluate to.
  */
  template<typename T>
  auto make_constant_reactor(T&& value) {
    return std::make_shared<constant_reactor<std::decay_t<T>>>(
      std::forward<T>(value));
  }

  //! Makes a constant reactor builder.
  /*!
    \param value The constant to evaluate to.
  */
  template<typename T>
  auto make_constant_reactor_builder(T&& value) {
    return std::make_unique<function_reactor_builder>(
      [value = std::forward<T>(value)] (auto& parameters, auto& t) {
        return make_constant_reactor(value);
      });
  }

  template<typename T>
  template<typename V>
  constant_reactor<T>::constant_reactor(V&& value)
      : m_sequence(-1),
        m_value(std::forward<V>(value)) {}

  template<typename T>
  base_reactor::update constant_reactor<T>::commit(int sequence) {
    if(m_sequence == -1) {
      m_sequence = sequence;
      return base_reactor::update::COMPLETE_WITH_EVAL;
    } else if(sequence > m_sequence) {
      return base_reactor::update::NONE;
    } else {
      return base_reactor::update::COMPLETE_WITH_EVAL;
    }
  }

  template<typename T>
  typename constant_reactor<T>::type constant_reactor<T>::eval() const {
    return m_value;
  }

namespace details {
  template<typename T>
  struct lift_helper {
    template<typename U>
    auto operator ()(U&& value) const {
      return std::static_pointer_cast<reactor<T>>(
        make_constant_reactor(std::forward<U>(value)));
    }
  };

  template<typename T>
  struct lift_helper<std::shared_ptr<T>> {
    template<typename U>
    decltype(auto) operator ()(U&& value) const {
      return std::forward<U>(value);
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
