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
  class chain_reactor : public reactor<T> {
    public:
      using type = typename reactor<T>::type;

      //! Constructs a chain reactor.
      /*!
        \param initial The reactor to initially evaluate to.
        \param continuation The reactor to evaluate to thereafter.
      */
      chain_reactor(std::shared_ptr<reactor<type>> initial,
        std::shared_ptr<reactor<type>> continuation);

      base_reactor::update commit(int sequence) override final;

      type eval() const override final;

    private:
      std::shared_ptr<reactor<type>> m_initial;
      std::shared_ptr<reactor<type>> m_continuation;
      reactor<type>* m_current;
      int m_transition_sequence;
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
          std::static_pointer_cast<reactor<T>>(parameters[0]),
          std::static_pointer_cast<reactor<T>>(parameters[1]));
      });
  }

  template<typename T>
  chain_reactor<T>::chain_reactor(std::shared_ptr<reactor<type>> initial,
      std::shared_ptr<reactor<type>> continuation)
      : m_initial(std::move(initial)),
        m_continuation(std::move(continuation)),
        m_current(&*m_initial),
        m_transition_sequence(-1) {}

  template<typename T>
  base_reactor::update chain_reactor<T>::commit(int sequence) {
    if(sequence == m_transition_sequence) {
      return base_reactor::update::EVAL;
    }
    auto state = m_current->commit(sequence);
    if(state == base_reactor::update::COMPLETE_EMPTY) {
      if(m_current == m_initial.get()) {
        m_current = m_continuation.get();
        return commit(sequence);
      } else {
        return m_initial->commit(sequence);
      }
    } else if(state == base_reactor::update::COMPLETE_EVAL) {
      if(m_current == m_initial.get()) {
        m_current = m_continuation.get();
        m_transition_sequence = sequence;
        return base_reactor::update::EVAL;
      }
    }
    return state;
  }

  template<typename T>
  typename chain_reactor<T>::type chain_reactor<T>::eval() const {
    return m_current->eval();
  }
}

#endif
