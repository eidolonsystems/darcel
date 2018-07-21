#ifndef DARCEL_COMMIT_REACTOR_HPP
#define DARCEL_COMMIT_REACTOR_HPP
#include <vector>
#include "darcel/reactors/reactor.hpp"
#include "darcel/reactors/reactors.hpp"

namespace darcel {

  //! Helper class used to determine the aggregate state of a list of reactors.
  class CommitReactor final : public Reactor<BaseReactor::Update> {
    public:

      //! Constructs a commit reactor.
      /*!
        \param children The reactors to aggregate.
      */
      CommitReactor(const std::vector<BaseReactor*>& children);

      BaseReactor::Update commit(int sequence) override;

      Type eval() const override;

    private:
      struct Child {
        BaseReactor* m_reactor;
        BaseReactor::Update m_state;

        Child(BaseReactor& reactor);
      };
      enum class State {
        INITIALIZING,
        EVALUATING
      };
      std::vector<Child> m_children;
      State m_state;
      int m_sequence;
      BaseReactor::Update m_update;
  };

  inline CommitReactor::Child::Child(BaseReactor& reactor)
      : m_reactor(&reactor),
        m_state(BaseReactor::Update::NONE) {}

  inline CommitReactor::CommitReactor(
      const std::vector<BaseReactor*>& children)
      : m_state(State::INITIALIZING),
        m_sequence(-1) {
    for(auto& child : children) {
      m_children.emplace_back(*child);
    }
  }

  inline BaseReactor::Update CommitReactor::commit(int sequence) {
    if(is_complete(m_update) || sequence == m_sequence) {
      return m_update;
    }
    if(m_state == State::INITIALIZING) {
      std::size_t initialization_count = 0;
      std::size_t completion_count = 0;
      for(auto& child : m_children) {
        if(child.m_state == Update::NONE) {
          child.m_state = child.m_reactor->commit(sequence);
          if(is_complete(child.m_state)) {
            ++completion_count;
          }
          if(has_eval(child.m_state)) {
            ++initialization_count;
          } else if(is_complete(child.m_state)) {
            m_update = Update::COMPLETE_EMPTY;
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
        m_update = Update::EVAL;
        if(completion_count == m_children.size()) {
          combine(m_update, Update::COMPLETE);
        } else {
          m_state = State::EVALUATING;
        }
      }
    } else if(m_state == State::EVALUATING) {
      m_update = Update::NONE;
      std::size_t completion_count = 0;
      for(auto& child : m_children) {
        if(is_complete(child.m_state)) {
          ++completion_count;
        } else {
          child.m_state = child.m_reactor->commit(sequence);
          if(has_eval(child.m_state)) {
            m_update = Update::EVAL;
          }
          if(is_complete(child.m_state)) {
            ++completion_count;
          }
        }
      }
      if(completion_count == m_children.size()) {
        m_update = Update::COMPLETE_EVAL;
      }
    }
    m_sequence = sequence;
    return m_update;
  }

  inline CommitReactor::Type CommitReactor::eval() const {
    return m_update;
  }
}

#endif
