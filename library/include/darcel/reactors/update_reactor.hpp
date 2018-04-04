#ifndef DARCEL_UPDATE_REACTOR_HPP
#define DARCEL_UPDATE_REACTOR_HPP
#include "darcel/reactors/constant_reactor.hpp"
#include "darcel/reactors/reactor.hpp"
#include "darcel/reactors/reactors.hpp"

namespace darcel {

  //! A reactor that evaluates to another reactor's most recent update.
  class update_reactor : public reactor<base_reactor::update> {
    public:

      //! Constructs an update reactor.
      /*!
        \param r The reactor to monitor for updates.
      */
      update_reactor(std::shared_ptr<base_reactor> r);

      base_reactor::update commit(int sequence) override final;

      type eval() const override final;

    private:
      std::shared_ptr<base_reactor> m_reactor;
      base_reactor::update m_value;
  };

  //! Makes an update reactor.
  /*!
    \param r The reactor to monitor for updates.
  */
  inline auto make_update_reactor(std::shared_ptr<base_reactor> r) {
    return std::make_shared<update_reactor>(std::move(r));
  }

  //! Makes an update reactor.
  /*!
    \param r The reactor to monitor for updates.
  */
  template<typename R>
  auto get_updates(R&& r) {
    return make_update_reactor(lift(std::forward<R>(r)));
  }

  inline update_reactor::update_reactor(std::shared_ptr<base_reactor> r)
      : m_reactor(std::move(r)),
        m_value(base_reactor::update::NONE) {}

  inline base_reactor::update update_reactor::commit(int sequence) {
    if(is_complete(m_value)) {
      return m_value;
    }
    m_value = m_reactor->commit(sequence);
    return m_value;
  }

  inline update_reactor::type update_reactor::eval() const {
    return m_value;
  }
}

#endif
