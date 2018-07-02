#ifndef DARCEL_CHAIN_REACTOR_HPP
#define DARCEL_CHAIN_REACTOR_HPP
#include <memory>
#include <utility>
#include "darcel/reactors/constant_reactor.hpp"
#include "darcel/reactors/reactor.hpp"
#include "darcel/reactors/reactor_builder.hpp"
#include "darcel/reactors/reactors.hpp"
#include "darcel/reactors/trigger.hpp"
#include "darcel/utilities/maybe.hpp"

namespace darcel {

  /*! \brief A reactor that produces values from one reactor until it completes
             and then produces values from another reactor.
      \tparam T The type of values to chain.
   */
  template<typename T>
  class chain_reactor final : public reactor<T> {
    public:
      using type = typename reactor<T>::type;

      //! Constructs a chain reactor.
      /*!
        \param initial The reactor to initially evaluate to.
        \param continuation The reactor to evaluate to thereafter.
      */
      chain_reactor(std::shared_ptr<reactor<type>> initial,
        std::shared_ptr<reactor<type>> continuation);

      base_reactor::update commit(int sequence) override;

      type eval() const override;

    private:
      enum state {
        INITIAL,
        TRANSITIONING,
        CONTINUATION
      };
      std::shared_ptr<reactor<type>> m_initial;
      std::shared_ptr<reactor<type>> m_continuation;
      reactor<type>* m_current;
      int m_sequence;
      state m_state;
      base_reactor::update m_update;
      bool m_has_eval;
  };

  //! Makes a chain reactor.
  /*!
    \param initial The reactor to initially evaluate to.
    \param continuation The reactor to evaluate to thereafter.
  */
  template<typename T, typename U>
  auto chain(T&& initial, U&& continuation) {
    auto initial_reactor = lift(std::forward<T>(initial));
    auto continuation_reactor = lift(std::forward<U>(continuation));
    using type = reactor_type_t<std::decay_t<decltype(initial_reactor)>>;
    return std::make_shared<chain_reactor<type>>(
      std::forward<decltype(initial_reactor)>(initial_reactor),
      std::forward<decltype(continuation_reactor)>(continuation_reactor));
  }

  //! Makes a chain reactor.
  /*!
    \param initial The reactor to initially evaluate to.
    \param continuation The reactor to evaluate to thereafter.
  */
  template<typename T>
  auto make_chain_reactor(std::shared_ptr<reactor<T>> initial,
      std::shared_ptr<reactor<T>> continuation) {
    return chain(std::move(initial), std::move(continuation));
  }

  //! Makes a chain reactor builder.
  template<typename T>
  auto make_chain_reactor_builder() {
    return std::make_unique<function_reactor_builder>(
      [] (auto& parameters, auto& t) {
        return make_chain_reactor(
          std::static_pointer_cast<reactor<T>>(parameters[0]->build(t)),
          std::static_pointer_cast<reactor<T>>(parameters[1]->build(t)));
      });
  }

  template<typename T>
  chain_reactor<T>::chain_reactor(std::shared_ptr<reactor<type>> initial,
      std::shared_ptr<reactor<type>> continuation)
      : m_initial(std::move(initial)),
        m_continuation(std::move(continuation)),
        m_current(&*m_initial),
        m_sequence(-1),
        m_state(state::INITIAL),
        m_update(base_reactor::update::NONE),
        m_has_eval(false) {}

  template<typename T>
  base_reactor::update chain_reactor<T>::commit(int sequence) {
    if(is_complete(m_update) || sequence == m_sequence) {
      return m_update;
    }
    if(m_state == state::INITIAL) {
      auto update = m_initial->commit(sequence);
      if(update == base_reactor::update::COMPLETE_EVAL) {
        m_state = state::TRANSITIONING;
        m_update = base_reactor::update::EVAL;
      } else if(update == base_reactor::update::COMPLETE_EMPTY) {
        m_state = state::CONTINUATION;
        m_current = m_continuation.get();
      } else {
        m_update = update;
      }
    } else if(m_state == state::TRANSITIONING) {
      m_state = state::CONTINUATION;
      m_current = m_continuation.get();
    }
    if(m_state == state::CONTINUATION) {
      auto update = m_continuation->commit(sequence);
      if(update == base_reactor::update::COMPLETE_EMPTY) {
        if(m_has_eval) {
          m_current = m_initial.get();
          m_update = base_reactor::update::COMPLETE_EVAL;
        }
      } else {
        m_update = update;
      }
    }
    m_sequence = sequence;
    m_has_eval |= has_eval(m_update);
    return m_update;
  }

  template<typename T>
  typename chain_reactor<T>::type chain_reactor<T>::eval() const {
    return m_current->eval();
  }
}

#endif
