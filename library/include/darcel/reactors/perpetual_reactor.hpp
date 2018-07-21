#ifndef DARCEL_PERPETUAL_REACTOR_HPP
#define DARCEL_PERPETUAL_REACTOR_HPP
#include "darcel/reactors/reactor.hpp"
#include "darcel/reactors/reactor_builder.hpp"
#include "darcel/reactors/reactors.hpp"

namespace darcel {

  //! A reactor that always evaluates to nothing, useful for building up
  //! reactors that need to iterate.
  class PerpetualReactor : public Reactor<void> {
    public:
      Update commit(int sequence) override;

      void eval() const override;
  };

  //! Makes a perpetual reactor.
  inline auto make_perpetual() {
    return std::make_shared<PerpetualReactor>();
  }

  //! Builds a perpetual reactor.
  inline auto make_perpetual_builder() {
    return std::make_unique<FunctionReactorBuilder>(
      [] (auto& parameters, auto& t) {
        return make_perpetual();
      });
  }

  inline BaseReactor::Update PerpetualReactor::commit(int sequence) {
    return Update::EVAL;
  }

  inline void PerpetualReactor::eval() const {}
}

#endif
