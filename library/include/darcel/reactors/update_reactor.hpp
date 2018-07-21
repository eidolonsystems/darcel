#ifndef DARCEL_UPDATE_REACTOR_HPP
#define DARCEL_UPDATE_REACTOR_HPP
#include "darcel/reactors/constant_reactor.hpp"
#include "darcel/reactors/reactor.hpp"
#include "darcel/reactors/reactors.hpp"

namespace darcel {

  //! A reactor that evaluates to another reactor's most recent update.
  class UpdateReactor final : public Reactor<BaseReactor::Update> {
    public:

      //! Constructs an update reactor.
      /*!
        \param r The reactor to monitor for updates.
      */
      UpdateReactor(std::shared_ptr<BaseReactor> r);

      BaseReactor::Update commit(int sequence) override;

      Type eval() const override;

    private:
      std::shared_ptr<BaseReactor> m_reactor;
      BaseReactor::Update m_value;
      BaseReactor::Update m_update;
  };

  //! Makes an update reactor.
  /*!
    \param r The reactor to monitor for updates.
  */
  inline auto make_update_reactor(std::shared_ptr<BaseReactor> r) {
    return std::make_shared<UpdateReactor>(std::move(r));
  }

  //! Makes an update reactor.
  /*!
    \param r The reactor to monitor for updates.
  */
  template<typename R>
  auto get_updates(R&& r) {
    return make_update_reactor(lift(std::forward<R>(r)));
  }

  inline UpdateReactor::UpdateReactor(std::shared_ptr<BaseReactor> r)
      : m_reactor(std::move(r)),
        m_update(Update::NONE) {}

  inline BaseReactor::Update UpdateReactor::commit(int sequence) {
    if(is_complete(m_update)) {
      return m_update;
    }
    m_value = m_reactor->commit(sequence);
    if(is_complete(m_value)) {
      m_update = Update::COMPLETE_EVAL;
    } else {
      m_update = Update::EVAL;
    }
    return m_update;
  }

  inline UpdateReactor::Type UpdateReactor::eval() const {
    return m_value;
  }
}

#endif
