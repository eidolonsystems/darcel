#ifndef DARCEL_CHAIN_REACTOR_HPP
#define DARCEL_CHAIN_REACTOR_HPP
#include <memory>
#include <utility>
#include "darcel/reactors/constant_reactor.hpp"
#include "darcel/reactors/reactor.hpp"
#include "darcel/reactors/reactors.hpp"
#include "darcel/reactors/trigger.hpp"
#include "darcel/utilities/maybe.hpp"

namespace darcel {

  /*! \brief A reactor that produces values from one reactor until it completes
             and then produces values from another reactor.
      \tparam T The type of values to chain.
   */
  template<typename T>
  class chain_reactor : public reactor<T> {
    public:
      using type = typename reactor<T>::type;

      //! Constructs a chain reactor.
      /*!
        \param initial The reactor to initially evaluate to.
        \param continuation The reactor to evaluate to thereafter.
        \param t The trigger used to transition between reactors.
      */
      chain_reactor(std::shared_ptr<reactor<type>> initial,
        std::shared_ptr<reactor<type>> continuation, trigger& t);

      base_reactor::update commit(int sequence) override final;

      type eval() const override final;

    private:
      enum class chain_state {
        INITIAL,
        TRANSITION,
        CONTINUATION,
      };
      std::shared_ptr<reactor<type>> m_initial;
      std::shared_ptr<reactor<type>> m_continuation;
      trigger* m_trigger;
      const reactor<type>* m_current;
      int m_transition_sequence;
      chain_state m_chain_state;
      int m_current_sequence;
      base_reactor::update m_update;
      base_reactor::update m_state;
  };

  //! Makes a chain_reactor.
  /*!
    \param initial The reactor to initially evaluate to.
    \param continuation The reactor to evaluate to thereafter.
    \param trigger The trigger used to transition between reactors.
  */
  template<typename T>
  auto make_chain_reactor(std::shared_ptr<reactor<T>> initial,
      std::shared_ptr<reactor<T>> continuation, trigger& t) {
    return make_chain_reactor(std::move(initial), std::move(continuation), t);
  }

  //! Makes a chain reactor.
  /*!
    \param initial The reactor to initially evaluate to.
    \param continuation The reactor to evaluate to thereafter.
    \param trigger The trigger used to transition between reactors.
  */
  template<typename T, typename U>
  auto chain(T&& initial, U&& continuation, trigger& t) {
    auto initial_reactor = lift(std::forward<T>(initial));
    auto continuation_reactor = lift(std::forward<U>(continuation));
    using type = reactor_type_t<std::decay_t<decltype(initial_reactor)>>;
    return std::make_shared<chain_reactor<type>>(
      std::forward<decltype(initial_reactor)>(initial_reactor),
      std::forward<decltype(continuation_reactor)>(continuation_reactor), t);
  }

  template<typename T>
  chain_reactor<T>::chain_reactor(std::shared_ptr<reactor<type>> initial,
      std::shared_ptr<reactor<type>> continuation, trigger& t)
      : m_initial(std::move(initial)),
        m_continuation(std::move(continuation)),
        m_trigger(&t),
        m_current(&*m_initial),
        m_transition_sequence(-1),
        m_chain_state(chain_state::INITIAL),
        m_current_sequence(-1),
        m_state(base_reactor::update::NONE) {}

  template<typename T>
  base_reactor::update chain_reactor<T>::commit(int sequence) {
    if(m_current_sequence == sequence) {
      return m_update;
    } else if(is_complete(m_state)) {
      return base_reactor::update::NONE;
    }
    if(m_chain_state == chain_state::INITIAL) {
      auto update = m_initial->commit(sequence);
      if(update == base_reactor::update::COMPLETE) {
        m_chain_state = chain_state::CONTINUATION;
        m_current_sequence = sequence;
        m_update = m_continuation->commit(m_current_sequence);
        if(m_update != base_reactor::update::COMPLETE) {
          m_current = &*m_continuation;
        }
        combine(m_state, m_update);
        return m_update;
      } else if(update == base_reactor::update::COMPLETE_WITH_EVAL) {
        m_chain_state = chain_state::TRANSITION;
        m_trigger->fetch_next_sequence(m_transition_sequence);
        m_current_sequence = sequence;
        m_update = base_reactor::update::EVAL;
        combine(m_state, base_reactor::update::EVAL);
        return base_reactor::update::EVAL;
      }
      m_current_sequence = sequence;
      m_update = update;
      combine(m_state, update);
      return update;
    } else if(m_chain_state == chain_state::TRANSITION) {
      if(sequence == m_transition_sequence) {
        m_chain_state = chain_state::CONTINUATION;
        m_current_sequence = sequence;
        m_update = m_continuation->commit(0);
        if(has_eval(m_update)) {
          m_current = &*m_continuation;
        }
        combine(m_state, m_update);
        return m_update;
      } else {
        return base_reactor::update::NONE;
      }
    } else {
      m_current_sequence = sequence;
      m_update = m_continuation->commit(sequence);
      if(has_eval(m_update)) {
        m_current = &*m_continuation;
      }
      combine(m_state, m_update);
      return m_update;
    }
  }

  template<typename T>
  typename chain_reactor<T>::type chain_reactor<T>::eval() const {
    return m_current->eval();
  }
}

#endif
