#ifndef DARCEL_TALLY_REACTOR_HPP
#define DARCEL_TALLY_REACTOR_HPP
#include <memory>
#include <utility>
#include "darcel/reactors/constant_reactor.hpp"
#include "darcel/reactors/reactor.hpp"
#include "darcel/reactors/reactor_builder.hpp"
#include "darcel/reactors/reactors.hpp"

namespace darcel {

  //! Countes the number of evaluations produced by a reactor.
  class TallyReactor final : public Reactor<int> {
    public:

      //! Constructs a tally reactor.
      /*!
        \param source The reactor to tally.
      */
      TallyReactor(std::shared_ptr<BaseReactor> source);

      Update commit(int sequence) override;

      Type eval() const override;

    private:
      std::shared_ptr<BaseReactor> m_source;
      int m_value;
      int m_sequence;
      Update m_update;
  };

  //! Makes a tally.
  /*!
    \param source The reactor to tally.
  */
  inline auto make_tally_reactor(std::shared_ptr<BaseReactor> source) {
    return std::make_shared<TallyReactor>(std::move(source));
  }

  //! Makes a tally.
  /*!
    \param source The reactor to tally.
  */
  template<typename R>
  inline auto tally(R&& source) {
    return make_tally_reactor(lift(std::forward<R>(source)));
  }

  //! Makes a tally reactor builder.
  inline auto make_tally_reactor_builder() {
    return std::make_unique<FunctionReactorBuilder>(
      [] (auto& parameters, auto& t) {
        return make_tally_reactor(parameters.front()->build(t));
      });
  }

  inline TallyReactor::TallyReactor(std::shared_ptr<BaseReactor> source)
      : m_source(std::move(source)),
        m_value(0),
        m_sequence(-1),
        m_update(Update::NONE) {}

  inline BaseReactor::Update TallyReactor::commit(int sequence) {
    if(sequence == m_sequence || is_complete(m_update)) {
      return m_update;
    }
    auto update = m_source->commit(sequence);
    if(is_complete(update)) {
      if(m_sequence == -1) {
        m_update = Update::COMPLETE_EVAL;
      } else {
        m_update = Update::COMPLETE;
      }
    } else {
      m_update = Update::EMPTY;
    }
    if(has_eval(update)) {
      ++m_value;
      combine(m_update, Update::EVAL);
    } else if(!is_complete(m_update)) {
      m_update = Update::NONE;
    }
    m_sequence = sequence;
    return m_update;
  }

  inline int TallyReactor::eval() const {
    return m_value;
  }
}

#endif
