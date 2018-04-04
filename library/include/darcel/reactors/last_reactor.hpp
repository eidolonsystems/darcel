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
  struct last_reactor_core {
    using type = T;
    std::optional<type> m_last;

    std::optional<type> operator ()(const type& value,
        base_reactor::update update) {
      if(update == base_reactor::update::COMPLETE_EVAL) {
        return value;
      } else if(update == base_reactor::update::NONE) {
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
  auto make_last_reactor(std::shared_ptr<reactor<T>> source) {
    return make_function_reactor(details::last_reactor_core<T>(),
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
    return std::make_unique<function_reactor_builder>(
      [] (auto& parameters, auto& t) {
        return make_last_reactor(std::static_pointer_cast<reactor<T>>(
          parameters.front()));
      });
  }
}

#endif
