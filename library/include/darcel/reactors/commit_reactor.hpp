#ifndef DARCEL_COMMIT_REACTOR_HPP
#define DARCEL_COMMIT_REACTOR_HPP
#include <vector>
#include "darcel/reactors/reactor.hpp"
#include "darcel/reactors/reactors.hpp"

namespace darcel {

  //! Helper class used to determine the aggregate state of a list of reactors.
  class commit_reactor : public reactor<base_reactor::update> {
    public:

      //! Constructs a commit reactor.
      /*!
        \param children The reactors to aggregate.
      */
      commit_reactor(const std::vector<base_reactor*>& children);

      base_reactor::update commit(int sequence) override final;

      type eval() const override final;

    private:
      struct child {
        base_reactor* m_reactor;
        bool m_is_initialized;
        bool m_is_complete;

        child(base_reactor& reactor);
      };
      std::vector<child> m_children;
      int m_current_sequence;
      base_reactor::update m_update;
      base_reactor::update m_state;
  };

  inline commit_reactor::child::child(base_reactor& reactor)
      : m_reactor(&reactor),
        m_is_initialized(false),
        m_is_complete(false) {}

  inline commit_reactor::commit_reactor(
      const std::vector<base_reactor*>& children)
      : m_current_sequence(-1),
        m_state(base_reactor::update::NONE) {
    for(auto& child : children) {
      m_children.emplace_back(*child);
    }
  }

  inline base_reactor::update commit_reactor::commit(int sequence) {
    if(m_current_sequence == sequence) {
      return m_update;
    } else if(is_complete(m_state)) {
      return base_reactor::update::NONE;
    }
    m_update =
      [&] {
        auto update = [&] {
          if(m_state == base_reactor::update::NONE) {
            return base_reactor::update::EVAL;
          } else {
            return base_reactor::update::NONE;
          }
        }();
        auto complete_count = 0;
        for(auto& child : m_children) {
          if(child.m_is_complete) {
            ++complete_count;
            continue;
          }
          auto child_update = child.m_reactor->commit(sequence);
          if(is_complete(child_update)) {
            ++complete_count;
            child.m_is_complete = true;
          }
          if(!child.m_is_initialized) {
            if(has_eval(child_update)) {
              child.m_is_initialized = true;
            } else if(child.m_is_complete) {
              update = base_reactor::update::COMPLETE;
            } else {
              update = base_reactor::update::NONE;
            }
          } else if(m_state != base_reactor::update::NONE) {
            if(has_eval(child_update)) {
              update = base_reactor::update::EVAL;
            }
          }
        }
        if(complete_count == static_cast<int>(m_children.size())) {
          combine(update, base_reactor::update::COMPLETE);
        }
        return update;
      }();
    m_current_sequence = sequence;
    combine(m_state, m_update);
    return m_update;
  }

  inline commit_reactor::type commit_reactor::eval() const {
    return m_update;
  }
}

#endif
