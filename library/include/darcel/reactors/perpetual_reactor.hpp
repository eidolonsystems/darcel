#ifndef DARCEL_PERPETUAL_REACTOR_HPP
#define DARCEL_PERPETUAL_REACTOR_HPP
#include "darcel/reactors/reactor.hpp"
#include "darcel/reactors/reactor_builder.hpp"
#include "darcel/reactors/reactors.hpp"

namespace darcel {

  //! A reactor that always evaluates to nothing, useful for building up
  //! reactors that need to iterate.
  class perpetual_reactor : public reactor<void> {
    public:
      update commit(int sequence) override;

      void eval() const override;
  };

  //! Makes a perpetual reactor.
  inline auto make_perpetual() {
    return std::make_shared<perpetual_reactor>();
  }

  //! Builds a perpetual reactor.
  inline auto make_perpetual_builder() {
    return std::make_unique<function_reactor_builder>(
      [] (auto& parameters, auto& t) {
        return make_perpetual();
      });
  }

  inline base_reactor::update perpetual_reactor::commit(int sequence) {
    return update::EVAL;
  }

  inline void perpetual_reactor::eval() const {}
}

#endif
