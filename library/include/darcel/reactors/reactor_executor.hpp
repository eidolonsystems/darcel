#ifndef DARCEL_REACTOR_EXECUTOR_HPP
#define DARCEL_REACTOR_EXECUTOR_HPP
#include <condition_variable>
#include <mutex>
#include "darcel/reactors/reactor.hpp"
#include "darcel/reactors/reactors.hpp"

namespace darcel {

  //! Provides a runtime environment to execute a single reactor.
  class reactor_executor {
    public:

      //! Constructs an executor with an initial sequence of 0.
      /*!
        \param r The reactor to execute.
      */
      reactor_executor(std::shared_ptr<base_reactor> r);

      //! Executes the reactors.
      void execute();

      //! Loads the next sequence, signalling an update is ready to commit.
      /*!
        \param sequence The variable to store the sequence in, passed in by
               reference to avoid potential race condition.
      */
      void fetch_next_sequence(int& sequence);

    private:
      std::shared_ptr<base_reactor> m_reactor;
      int m_current_sequence;
      int m_next_sequence;
      std::condition_variable m_update_condition;
      std::mutex m_mutex;

      reactor_executor(const reactor_executor&) = delete;
      reactor_executor& operator =(const reactor_executor&) = delete;
  };

  inline reactor_executor::reactor_executor(std::shared_ptr<base_reactor> r)
      : m_reactor(std::move(r)),
        m_current_sequence(0),
        m_next_sequence(0) {}

  inline void reactor_executor::execute() {
    while(true) {
      auto update = m_reactor->commit(m_current_sequence);
      if(is_complete(update)) {
        return;
      }
      ++m_current_sequence;
      auto lock = std::unique_lock<std::mutex>(m_mutex);
      while(m_current_sequence > m_next_sequence) {
        m_update_condition.wait(lock);
      }
    }
  }

  inline void reactor_executor::fetch_next_sequence(int& sequence) {
    sequence = ++m_next_sequence;
    m_update_condition.notify_one();
  }
}

#endif
