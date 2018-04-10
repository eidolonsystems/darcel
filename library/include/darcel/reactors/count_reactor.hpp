#ifndef DARCEL_COUNT_REACTOR_HPP
#define DARCEL_COUNT_REACTOR_HPP
#include <algorithm>
#include <memory>
#include <utility>
#include "darcel/reactors/function_reactor.hpp"
#include "darcel/reactors/perpetual_reactor.hpp"
#include "darcel/reactors/reactor.hpp"
#include "darcel/reactors/reactor_builder.hpp"
#include "darcel/reactors/reactors.hpp"
#include "darcel/reactors/update_reactor.hpp"
#include "darcel/utilities/maybe.hpp"

namespace darcel {

  //! Makes a reactor that counts integers from a starting value to an end value
  //! (inclusive).
  /*!
    \param start The number to start counting from.
    \param end The number to count up to (inclusive).
  */
  inline auto make_count_reactor(std::shared_ptr<reactor<int>> start,
      std::shared_ptr<reactor<int>> end) {
    auto start_updates = get_updates(start);
    auto end_updates = get_updates(end);
    auto looper = make_perpetual();
    return make_function_reactor(
      [=, value = 0, is_counting = false](int start,
          base_reactor::update start_state, int end,
          base_reactor::update end_state, maybe<void> ignored) mutable {
        if(!is_counting) {
          value = start - 1;
          is_counting = true;
        }
        auto c = std::max(start, value + 1);
        if(c > end) {
          if(is_complete(start_state) && is_complete(end_state)) {
            return function_evaluation<int>(base_reactor::update::COMPLETE);
          }
          return function_evaluation<int>(base_reactor::update::NONE);
        }
        value = c;
        if(value == end && is_complete(start_state) && is_complete(end_state)) {
          return make_function_evaluation(value,
            base_reactor::update::COMPLETE);
        }
        return function_evaluation<int>(value);
      }, std::move(start), std::move(start_updates), std::move(end),
      std::move(end_updates), looper);
  }

  //! Makes a reactor that counts integers from a starting value to an end value
  //! (inclusive).
  /*!
    \param start The number to start counting from.
    \param end The number to count up to (inclusive).
  */
  template<typename S, typename E>
  auto count(S&& start, E&& end) {
    return make_count_reactor(lift(std::forward<S>(start)),
      lift(std::forward<E>(end)));
  }

  //! Builds a count reactor.
  inline auto make_count_builder() {
    return std::make_unique<function_reactor_builder>(
      [] (auto& parameters, auto& t) {
        return make_count_reactor(
          std::static_pointer_cast<reactor<int>>(parameters[0]->build(t)),
          std::static_pointer_cast<reactor<int>>(parameters[1]->build(t)));
      });
  }
}

#endif