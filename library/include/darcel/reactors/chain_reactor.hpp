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
  class ChainReactor final : public Reactor<T> {
    public:
      using Type = typename Reactor<T>::Type;

      //! Constructs a chain reactor.
      /*!
        \param initial The reactor to initially evaluate to.
        \param continuation The reactor to evaluate to thereafter.
      */
      ChainReactor(std::shared_ptr<Reactor<Type>> initial,
        std::shared_ptr<Reactor<Type>> continuation);

      BaseReactor::Update commit(int sequence) override;

      Type eval() const override;

    private:
      enum State {
        INITIAL,
        TRANSITIONING,
        CONTINUATION
      };
      std::shared_ptr<Reactor<Type>> m_initial;
      std::shared_ptr<Reactor<Type>> m_continuation;
      Reactor<Type>* m_current;
      int m_sequence;
      State m_state;
      BaseReactor::Update m_update;
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
    using Type = reactor_type_t<std::decay_t<decltype(initial_reactor)>>;
    return std::make_shared<ChainReactor<Type>>(
      std::forward<decltype(initial_reactor)>(initial_reactor),
      std::forward<decltype(continuation_reactor)>(continuation_reactor));
  }

  //! Makes a chain reactor.
  /*!
    \param initial The reactor to initially evaluate to.
    \param continuation The reactor to evaluate to thereafter.
  */
  template<typename T>
  auto make_chain_reactor(std::shared_ptr<Reactor<T>> initial,
      std::shared_ptr<Reactor<T>> continuation) {
    return chain(std::move(initial), std::move(continuation));
  }

  //! Makes a chain reactor builder.
  template<typename T>
  auto make_chain_reactor_builder() {
    return std::make_unique<FunctionReactorBuilder>(
      [] (auto& parameters, auto& t) {
        return make_chain_reactor(
          std::static_pointer_cast<Reactor<T>>(parameters[0]->build(t)),
          std::static_pointer_cast<Reactor<T>>(parameters[1]->build(t)));
      });
  }

  template<typename T>
  ChainReactor<T>::ChainReactor(std::shared_ptr<Reactor<Type>> initial,
      std::shared_ptr<Reactor<Type>> continuation)
      : m_initial(std::move(initial)),
        m_continuation(std::move(continuation)),
        m_current(&*m_initial),
        m_sequence(-1),
        m_state(State::INITIAL),
        m_update(BaseReactor::Update::NONE),
        m_has_eval(false) {}

  template<typename T>
  BaseReactor::Update ChainReactor<T>::commit(int sequence) {
    if(is_complete(m_update) || sequence == m_sequence) {
      return m_update;
    }
    if(m_state == State::INITIAL) {
      auto update = m_initial->commit(sequence);
      if(update == BaseReactor::Update::COMPLETE_EVAL) {
        m_state = State::TRANSITIONING;
        m_update = BaseReactor::Update::EVAL;
      } else if(update == BaseReactor::Update::COMPLETE_EMPTY) {
        m_state = State::CONTINUATION;
        m_current = m_continuation.get();
      } else {
        m_update = update;
      }
    } else if(m_state == State::TRANSITIONING) {
      m_state = State::CONTINUATION;
      m_current = m_continuation.get();
    }
    if(m_state == State::CONTINUATION) {
      auto update = m_continuation->commit(sequence);
      if(update == BaseReactor::Update::COMPLETE_EMPTY) {
        if(m_has_eval) {
          m_current = m_initial.get();
          m_update = BaseReactor::Update::COMPLETE_EVAL;
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
  typename ChainReactor<T>::Type ChainReactor<T>::eval() const {
    return m_current->eval();
  }
}

#endif
