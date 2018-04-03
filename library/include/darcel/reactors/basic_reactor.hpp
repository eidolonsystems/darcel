#ifndef DARCEL_BASIC_REACTOR_HPP
#define DARCEL_BASIC_REACTOR_HPP
#include <deque>
#include <exception>
#include <mutex>
#include "darcel/reactors/reactor.hpp"
#include "darcel/reactors/reactor_unavailable_exception.hpp"
#include "darcel/reactors/reactors.hpp"
#include "darcel/reactors/trigger.hpp"
#include "darcel/utilities/maybe.hpp"

namespace darcel {

  //! A reactor that is programmatically updated.
  /*!
    \tparam T the type of value to evaluate.
  */
  template<typename T>
  class basic_reactor : public reactor<T> {
    public:
      using type = reactor_type_t<reactor<T>>;

      //! Constructs a basic reactor.
      /*!
        \param t The trigger used to indicate updates.
      */
      basic_reactor(trigger& t);

      //! Updates this reactor with a value.
      /*!
        \param value The value to update.
      */
      void set_value(type value);

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

      base_reactor::update commit(int sequence) override final;

      type eval() const override final;

    private:
      struct entry {
        type m_value;
        int m_sequence;

        entry(type value);
      };
      std::mutex m_mutex;
      trigger* m_trigger;
      maybe<type> m_value;
      int m_sequence;
      update m_update;
      std::deque<entry> m_queue;
      std::exception_ptr m_exception;
      int m_final_sequence;
  };

  //! Makes a basic reactor.
  /*!
    \param t The trigger used to indicate updates.
  */
  template<typename T>
  auto make_basic_reactor(trigger& t) {
    return std::make_shared<basic_reactor<T>>(t);
  }

  template<typename T>
  basic_reactor<T>::entry::entry(type value)
      : m_value(std::move(value)) {}

  template<typename T>
  basic_reactor<T>::basic_reactor(trigger& t)
      : m_trigger(&t),
        m_value(std::make_exception_ptr(reactor_unavailable_exception())),
        m_sequence(-1),
        m_update(update::NONE),
        m_final_sequence(-1) {}

  template<typename T>
  void basic_reactor<T>::set_value(type value) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_queue.emplace_back(std::move(value));
    m_trigger->fetch_next_sequence(m_queue.back().m_sequence);
  }

  template<typename T>
  void basic_reactor<T>::set_complete() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_trigger->fetch_next_sequence(m_final_sequence);
  }

  template<typename T>
  void basic_reactor<T>::set_complete(std::exception_ptr e) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_exception = std::move(e);
    m_trigger->fetch_next_sequence(m_final_sequence);
  }

  template<typename T>
  template<typename E>
  void basic_reactor<T>::set_complete(const E& e) {
    set_complete(std::make_exception_ptr(e));
  }

  template<typename T>
  base_reactor::update basic_reactor<T>::commit(int sequence) {
    if(sequence == m_sequence) {
      return m_update;
    }
    std::lock_guard<std::mutex> lock(m_mutex);
    if(sequence == m_final_sequence) {
      m_sequence = m_final_sequence;
      if(m_exception != nullptr) {
        m_value = std::move(m_exception);
        m_update = update::COMPLETE_WITH_EVAL;
      } else {
        m_update = update::COMPLETE;
      }
      return m_update;
    }
    if(m_queue.empty() || m_queue.front().m_sequence != sequence) {
      return update::NONE;
    }
    m_value = std::move(m_queue.front().m_value);
    m_queue.pop_front();
    m_sequence = sequence;
    m_update = update::EVAL;
    return update::EVAL;
  }

  template<typename T>
  typename basic_reactor<T>::type basic_reactor<T>::eval() const {
    return m_value;
  }
}

#endif
