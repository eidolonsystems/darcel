#ifndef DARCEL_TRIGGER_HPP
#define DARCEL_TRIGGER_HPP
#include <condition_variable>
#include <mutex>
#include "darcel/reactors/reactors.hpp"

namespace darcel {

  //! Allows reactors to trigger updates and manages their sequence numbers.
  class trigger {
    public:

      //! Constructs a trigger with an initial sequence of 0.
      trigger();

      //! Returns the current sequence number.
      int get_current_sequence() const;

      //! Loads the next sequence, signalling an update is ready to commit.
      /*!
        \param sequence The variable to store the sequence in, passed in by
               reference to avoid potential race condition.
      */
      void fetch_next_sequence(int& sequence);

      //! Waits for an update.
      /*!
        \return The last sequence number produced.
      */
      int wait();

   private:
      int m_current_sequence;
      int m_next_sequence;
      std::condition_variable m_update_condition;
      std::mutex m_mutex;

      trigger(const trigger&) = delete;
      trigger& operator =(const trigger&) = delete;
  };

  inline trigger::trigger()
      : m_current_sequence(0),
        m_next_sequence(0) {}

  inline int trigger::get_current_sequence() const {
    return m_current_sequence;
  }

  inline void trigger::fetch_next_sequence(int& sequence) {
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      sequence = ++m_next_sequence;
    }
    m_update_condition.notify_one();
  }

  inline int trigger::wait() {
    auto lock = std::unique_lock<std::mutex>(m_mutex);
    while(m_current_sequence >= m_next_sequence) {
      m_update_condition.wait(lock);
    }
    auto sequence = m_current_sequence;
    ++m_current_sequence;
    return sequence;
  }
}

#endif
