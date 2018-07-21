#ifndef DARCEL_TRIGGER_HPP
#define DARCEL_TRIGGER_HPP
#include <condition_variable>
#include <mutex>
#include "darcel/reactors/reactors.hpp"

namespace darcel {

  //! Allows reactors to trigger updates and manages their sequence numbers.
  class Trigger {
    public:

      //! Constructs a trigger with an initial sequence of 0.
      Trigger();

      //! Returns the current sequence number.
      int get_current_sequence() const;

      //! Signals an update is ready to commit.
      void signal_update();

      //! Waits for an update.
      /*!
        \param sequence The current sequence number;
        \return The last sequence number produced.
      */
      int wait(int sequence);

   private:
      int m_current_sequence;
      std::condition_variable m_update_condition;
      std::mutex m_mutex;

      Trigger(const Trigger&) = delete;
      Trigger& operator =(const Trigger&) = delete;
  };

  inline Trigger::Trigger()
      : m_current_sequence(0) {}

  inline int Trigger::get_current_sequence() const {
    return m_current_sequence;
  }

  inline void Trigger::signal_update() {
    m_update_condition.notify_one();
  }

  inline int Trigger::wait(int sequence) {
    auto lock = std::unique_lock<std::mutex>(m_mutex);
    while(sequence < m_current_sequence) {
      m_update_condition.wait(lock);
    }
    return m_current_sequence++;
  }
}

#endif
