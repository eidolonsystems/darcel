#ifndef DARCEL_REACTOR_EXECUTOR_HPP
#define DARCEL_REACTOR_EXECUTOR_HPP
#include "darcel/reactors/reactor.hpp"
#include "darcel/reactors/reactors.hpp"
#include "darcel/reactors/trigger.hpp"

namespace darcel {

  //! Provides a runtime environment to execute a single reactor.
  class ReactorExecutor {
    public:

      //! Constructs an executor with an initial sequence of 0.
      /*!
        \param r The reactor to execute.
        \param t The trigger used to indicate reactor updates.
      */
      ReactorExecutor(std::shared_ptr<BaseReactor> r, Trigger& t);

      //! Executes the reactors.
      void execute();

    private:
      std::shared_ptr<BaseReactor> m_reactor;
      Trigger* m_trigger;
      int m_sequence;

      ReactorExecutor(const ReactorExecutor&) = delete;
      ReactorExecutor& operator =(const ReactorExecutor&) = delete;
  };

  inline ReactorExecutor::ReactorExecutor(std::shared_ptr<BaseReactor> r,
      Trigger& t)
      : m_reactor(std::move(r)),
        m_trigger(&t),
        m_sequence(m_trigger->get_current_sequence()) {}

  inline void ReactorExecutor::execute() {
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
