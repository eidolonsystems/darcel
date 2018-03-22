#ifndef DARCEL_REACTOR_EXECUTOR_HPP
#define DARCEL_REACTOR_EXECUTOR_HPP
#include <condition_variable>
#include <mutex>
#include "darcel/reactors/reactor.hpp"
#include "darcel/reactors/reactors.hpp"
#include "darcel/reactors/trigger.hpp"

namespace darcel {

  //! Provides a runtime environment to execute a single reactor.
  class reactor_executor {
    public:

      //! Constructs an executor with an initial sequence of 0.
      /*!
        \param r The reactor to execute.
        \param t The trigger used to indicate reactor updates.
      */
      reactor_executor(std::shared_ptr<base_reactor> r, trigger& t);

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
      trigger* m_trigger;

      reactor_executor(const reactor_executor&) = delete;
      reactor_executor& operator =(const reactor_executor&) = delete;
  };

  inline reactor_executor::reactor_executor(std::shared_ptr<base_reactor> r,
      trigger& t)
      : m_reactor(std::move(r)),
        m_trigger(&t) {}

  inline void reactor_executor::execute() {
    while(true) {
      auto update = m_reactor->commit(m_trigger->get_current_sequence());
      if(is_complete(update)) {
        return;
      }
      m_trigger->wait();
    }
  }
}

#endif
