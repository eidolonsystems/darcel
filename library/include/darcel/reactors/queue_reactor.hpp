#ifndef DARCEL_QUEUE_REACTOR_HPP
#define DARCEL_QUEUE_REACTOR_HPP
#include <deque>
#include <exception>
#include <mutex>
#include <optional>
#include "darcel/reactors/reactor.hpp"
#include "darcel/reactors/reactor_unavailable_exception.hpp"
#include "darcel/reactors/reactors.hpp"
#include "darcel/reactors/trigger.hpp"
#include "darcel/utilities/maybe.hpp"

namespace darcel {

  //! A reactor that emits a queue of values.
  /*!
    \tparam T the type of value to queue.
  */
  template<typename T>
  class QueueReactor final : public Reactor<T> {
    public:
      using Type = reactor_type_t<Reactor<T>>;

      //! Constructs a queue reactor.
      /*!
        \param t The trigger used to indicate updates.
      */
      QueueReactor(Trigger& t);

      //! Pushes a value to the queue.
      /*!
        \param value The value to push.
      */
      void push(Type value);

      //! Brings this reactor to a completion state.
      void set_complete();

      //! Brings this reactor to a completion state by throwing an exception.
      /*!
        \param e The exception to throw.
      */
      void set_complete(std::exception_ptr e);

      //! Brings this reactor to a completion state by throwing an exception.
      /*!
        \param e The exception to throw.
      */
      template<typename E>
      void set_complete(const E& e);

      BaseReactor::Update commit(int sequence) override;

      Type eval() const override;

    private:
      std::mutex m_mutex;
      Trigger* m_trigger;
      Maybe<Type> m_value;
      int m_sequence;
      Update m_state;
      std::deque<Type> m_queue;
      std::optional<std::exception_ptr> m_exception;
  };

  //! Makes a queue reactor.
  /*!
    \param t The trigger used to indicate updates.
  */
  template<typename T>
  auto make_queue_reactor(Trigger& t) {
    return std::make_shared<QueueReactor<T>>(t);
  }

  template<typename T>
  QueueReactor<T>::QueueReactor(Trigger& t)
      : m_trigger(&t),
        m_value(std::make_exception_ptr(ReactorUnavailableException())),
        m_sequence(-1),
        m_state(Update::NONE) {}

  template<typename T>
  void QueueReactor<T>::push(Type value) {
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      m_queue.emplace_back(std::move(value));
    }
    m_trigger->signal_update();
  }

  template<typename T>
  void QueueReactor<T>::set_complete() {
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      m_exception.emplace();
    }
    m_trigger->signal_update();
  }

  template<typename T>
  void QueueReactor<T>::set_complete(std::exception_ptr e) {
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      m_exception.emplace(std::move(e));
    }
    m_trigger->signal_update();
  }

  template<typename T>
  template<typename E>
  void QueueReactor<T>::set_complete(const E& e) {
    set_complete(std::make_exception_ptr(e));
  }

  template<typename T>
  BaseReactor::Update QueueReactor<T>::commit(int sequence) {
    if(is_complete(m_state) || sequence == m_sequence) {
      return m_state;
    }
    std::lock_guard<std::mutex> lock(m_mutex);
    if(m_queue.empty()) {
      if(m_exception.has_value()) {
        if(*m_exception == nullptr) {
          combine(m_state, BaseReactor::Update::COMPLETE);
        } else {
          m_value = std::move(*m_exception);
          m_state = BaseReactor::Update::COMPLETE_EVAL;
        }
      } else {
        m_state = BaseReactor::Update::NONE;
      }
    } else {
      m_value = std::move(m_queue.front());
      m_queue.pop_front();
      m_state = BaseReactor::Update::EVAL;
    }
    m_sequence = sequence;
    return m_state;
  }

  template<typename T>
  typename QueueReactor<T>::Type QueueReactor<T>::eval() const {
    return m_value;
  }
}

#endif
