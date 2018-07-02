#ifndef DARCEL_COMMIT_REACTOR_HPP
#define DARCEL_COMMIT_REACTOR_HPP
#include <vector>
#include "darcel/reactors/reactor.hpp"
#include "darcel/reactors/reactors.hpp"

namespace darcel {

  //! Helper class used to determine the aggregate state of a list of reactors.
  class commit_reactor final : public reactor<base_reactor::update> {
    public:

      //! Constructs a commit reactor.
      /*!
        \param children The reactors to aggregate.
      */
      commit_reactor(const std::vector<base_reactor*>& children);

      base_reactor::update commit(int sequence) override;

      type eval() const override;

    private:
      struct child {
        base_reactor* m_reactor;
        base_reactor::update m_state;

        child(base_reactor& reactor);
      };
      enum class state {
        INITIALIZING,
        EVALUATING
      };
      std::vector<child> m_children;
      state m_state;
      int m_sequence;
      base_reactor::update m_update;
  };

  inline commit_reactor::child::child(base_reactor& reactor)
      : m_reactor(&reactor),
        m_state(base_reactor::update::NONE) {}

  inline commit_reactor::commit_reactor(
      const std::vector<base_reactor*>& children)
      : m_state(state::INITIALIZING),
        m_sequence(-1) {
    for(auto& child : children) {
      m_children.emplace_back(*child);
    }
  }

  inline base_reactor::update commit_reactor::commit(int sequence) {
    if(is_complete(m_update) || sequence == m_sequence) {
      return m_update;
    }
    if(m_state == state::INITIALIZING) {
      std::size_t initialization_count = 0;
      std::size_t completion_count = 0;
      for(auto& child : m_children) {
        if(child.m_state == update::NONE) {
          child.m_state = child.m_reactor->commit(sequence);
          if(is_complete(child.m_state)) {
            ++completion_count;
          }
          if(has_eval(child.m_state)) {
            ++initialization_count;
          } else if(is_complete(child.m_state)) {
            m_update = update::COMPLETE_EMPTY;
            break;
          }
        } else {
          ++initialization_count;
          auto state = child.m_reactor->commit(sequence);
          if(is_complete(state)) {
            ++completion_count;
            child.m_state = state;
          }
        }
      }
      if(initialization_count == m_children.size()) {
        m_update = update::EVAL;
        if(completion_count == m_children.size()) {
          combine(m_update, update::COMPLETE);
        } else {
          m_state = state::EVALUATING;
        }
      }
    } else if(m_state == state::EVALUATING) {
      m_update = update::NONE;
      std::size_t completion_count = 0;
      for(auto& child : m_children) {
        if(is_complete(child.m_state)) {
          ++completion_count;
        } else {
          child.m_state = child.m_reactor->commit(sequence);
          if(has_eval(child.m_state)) {
            m_update = update::EVAL;
          }
          if(is_complete(child.m_state)) {
            ++completion_count;
          }
        }
      }
      if(completion_count == m_children.size()) {
        m_update = update::COMPLETE_EVAL;
      }
    }
    m_sequence = sequence;
    return m_update;
  }

  inline commit_reactor::type commit_reactor::eval() const {
    return m_update;
  }
}

#endif
