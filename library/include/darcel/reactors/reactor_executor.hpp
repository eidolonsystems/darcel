#ifndef DARCEL_REACTOR_EXECUTOR_HPP
#define DARCEL_REACTOR_EXECUTOR_HPP
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

    private:
      std::shared_ptr<base_reactor> m_reactor;
      trigger* m_trigger;
      int m_sequence;

      reactor_executor(const reactor_executor&) = delete;
      reactor_executor& operator =(const reactor_executor&) = delete;
  };

  inline reactor_executor::reactor_executor(std::shared_ptr<base_reactor> r,
      trigger& t)
      : m_reactor(std::move(r)),
        m_trigger(&t),
        m_sequence(m_trigger->get_current_sequence()) {}

  inline void reactor_executor::execute() {
    while(true) {
      auto update = m_reactor->commit(m_sequence);
      ++m_sequence;
      if(is_complete(update)) {
        return;
      }
      m_trigger->wait(m_sequence);
    }
  }
}

#endif
