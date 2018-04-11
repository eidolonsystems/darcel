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
  class tally_reactor : public reactor<int> {
    public:

      //! Constructs a tally reactor.
      /*!
        \param source The reactor to tally.
      */
      tally_reactor(std::shared_ptr<base_reactor> source);

      update commit(int sequence) override final;

      type eval() const override final;

    private:
      std::shared_ptr<base_reactor> m_source;
      int m_value;
      int m_sequence;
      update m_update;
  };

  //! Makes a tally.
  /*!
    \param source The reactor to tally.
  */
  inline auto make_tally_reactor(std::shared_ptr<base_reactor> source) {
    return std::make_shared<tally_reactor>(std::move(source));
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
    return std::make_unique<function_reactor_builder>(
      [] (auto& parameters, auto& t) {
        return make_tally_reactor(parameters.front()->build(t));
      });
  }

  inline tally_reactor::tally_reactor(std::shared_ptr<base_reactor> source)
      : m_source(std::move(source)),
        m_value(0),
        m_sequence(-1),
        m_update(update::NONE) {}

  inline base_reactor::update tally_reactor::commit(int sequence) {
    if(sequence == m_sequence || is_complete(m_update)) {
      return m_update;
    }
    auto update = m_source->commit(sequence);
    if(is_complete(update)) {
      if(m_sequence == -1) {
        m_update = update::COMPLETE_EVAL;
      } else {
        m_update = update::COMPLETE;
      }
    } else {
      m_update = update::EMPTY;
    }
    if(has_eval(update)) {
      ++m_value;
      combine(m_update, update::EVAL);
    } else if(!is_complete(m_update)) {
      m_update = update::NONE;
    }
    m_sequence = sequence;
    return m_update;
  }

  inline int tally_reactor::eval() const {
    return m_value;
  }
}

#endif
