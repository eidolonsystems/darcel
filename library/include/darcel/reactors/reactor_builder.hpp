#ifndef DARCEL_REACTOR_BUILDER_HPP
#define DARCEL_REACTOR_BUILDER_HPP
#include <functional>
#include <memory>
#include <vector>
#include "darcel/reactors/reactors.hpp"

namespace darcel {

  //! Builds a reactor composed of a list of sub-reactors.
  class ReactorBuilder {
    public:
      virtual ~ReactorBuilder() = default;

      //! Builds a reactor with no sub-reactors.
      /*!
        \param t The trigger used to indicate updates.
        \return The reactor represented by this builder composed of the
                specified parameters.
      */
      std::shared_ptr<BaseReactor> build(Trigger& t) const;

      //! Builds a reactor from a list of sub-reactors.
      /*!
        \param parameters The list of parameters to the reactor to be built.
        \param t The trigger used to indicate updates.
        \return The reactor represented by this builder composed of the
                specified parameters.
      */
      virtual std::shared_ptr<BaseReactor> build(
        const std::vector<std::shared_ptr<ReactorBuilder>>& parameters,
        Trigger& t) const = 0;

    protected:

      //! Constructs an empty ReactorBuilder.
      ReactorBuilder() = default;

    private:
      ReactorBuilder(const ReactorBuilder&) = delete;
      ReactorBuilder& operator =(const ReactorBuilder&) = delete;
  };

  //! Implements a reactor builder using a function.
  class FunctionReactorBuilder final : public ReactorBuilder {
    public:

      //! Constructs a function reactor builder.
      /*!
        \param f The function used to build the reactor.
      */
      template<typename F>
      FunctionReactorBuilder(F&& f);

      std::shared_ptr<BaseReactor> build(
        const std::vector<std::shared_ptr<ReactorBuilder>>& parameters,
        Trigger& t) const override;

    private:
      std::function<std::shared_ptr<BaseReactor> (
        const std::vector<std::shared_ptr<ReactorBuilder>>&, Trigger&)>
        m_function;
  };

  //! Makes a reactor builder that passes through a reactor.
  /*!
    \param r The reactor to pass through.
  */
  inline auto make_passthrough_reactor_builder(
      std::shared_ptr<BaseReactor> r) {
    return std::make_unique<FunctionReactorBuilder>(
      [=] (auto& parameters, auto& t) {
        return r;
      });
  }

  template<typename F>
  FunctionReactorBuilder::FunctionReactorBuilder(F&& f)
      : m_function(std::forward<F>(f)) {}

  inline std::shared_ptr<BaseReactor> FunctionReactorBuilder::build(
      const std::vector<std::shared_ptr<ReactorBuilder>>& parameters,
      Trigger& t) const {
    return m_function(parameters, t);
  }

  inline std::shared_ptr<BaseReactor> ReactorBuilder::build(
      Trigger& t) const {
    return build({}, t);
  }
}

#endif
