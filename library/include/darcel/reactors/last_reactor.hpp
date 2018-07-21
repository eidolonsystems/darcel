#ifndef DARCEL_LAST_REACTOR_HPP
#define DARCEL_LAST_REACTOR_HPP
#include <memory>
#include <optional>
#include <utility>
#include "darcel/reactors/constant_reactor.hpp"
#include "darcel/reactors/function_reactor.hpp"
#include "darcel/reactors/reactors.hpp"
#include "darcel/reactors/update_reactor.hpp"

namespace darcel {
namespace details {
  template<typename T>
  struct LastReactorCore {
    using Type = T;
    std::optional<Type> m_last;

    std::optional<Type> operator ()(const Type& value,
        BaseReactor::Update update) {
      if(update == BaseReactor::Update::COMPLETE_EVAL) {
        return value;
      } else if(update == BaseReactor::Update::NONE) {
        return std::nullopt;
      } else if(has_eval(update)) {
        m_last = value;
        return std::nullopt;
      } else {
        return m_last;
      }
    }
  };
}

  //! Makes a reactor that evaluates to the last value it receives from its
  //! source.
  /*!
    \param source The source that will provide the value to evaluate to.
  */
  template<typename T>
  auto make_last_reactor(std::shared_ptr<Reactor<T>> source) {
    return make_function_reactor(details::LastReactorCore<T>(),
      std::move(source), get_updates(source));
  }

  //! Makes a reactor that evaluates to the last value it receives from its
  //! source.
  /*!
    \param source The source that will provide the value to evaluate to.
  */
  template<typename R>
  auto last(R&& source) {
    return make_last_reactor(lift(std::forward<R>(source)));
  }

  //! Builds a last reactor.
  template<typename T>
  auto make_last_reactor_builder() {
    return std::make_unique<FunctionReactorBuilder>(
      [] (auto& parameters, auto& t) {
        return make_last_reactor(
          std::static_pointer_cast<Reactor<T>>(parameters.front()->build(t)));
      });
  }
}

#endif
