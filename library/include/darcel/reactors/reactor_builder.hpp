#ifndef DARCEL_REACTOR_BUILDER_HPP
#define DARCEL_REACTOR_BUILDER_HPP
#include <functional>
#include <memory>
#include <vector>
#include "darcel/reactors/reactors.hpp"

namespace darcel {

  //! Builds a reactor composed of a list of sub-reactors.
  class reactor_builder {
    public:
      virtual ~reactor_builder() = default;

      //! Builds a reactor with no sub-reactors.
      /*!
        \param t The trigger used to indicate updates.
        \return The reactor represented by this builder composed of the
                specified parameters.
      */
      std::shared_ptr<base_reactor> build(trigger& t) const;

      //! Builds a reactor from a list of sub-reactors.
      /*!
        \param parameters The list of sub-reactors that serve as parameters to
               the reactor to be built.
        \param t The trigger used to indicate updates.
        \return The reactor represented by this builder composed of the
                specified parameters.
      */
      virtual std::shared_ptr<base_reactor> build(
        const std::vector<std::shared_ptr<base_reactor>>& parameters,
        trigger& t) const = 0;

    protected:

      //! Constructs an empty reactor_builder.
      reactor_builder() = default;

    private:
      reactor_builder(const reactor_builder&) = delete;
      reactor_builder& operator =(const reactor_builder&) = delete;
  };

  //! Implements a reactor builder using a function.
  class function_reactor_builder : public reactor_builder {
    public:

      //! Constructs a function reactor builder.
      /*!
        \param f The function used to build the reactor.
      */
      template<typename F>
      function_reactor_builder(F&& f);

      std::shared_ptr<base_reactor> build(
        const std::vector<std::shared_ptr<base_reactor>>& parameters,
        trigger& t) const override final;

    private:
      std::function<std::shared_ptr<base_reactor> (
        const std::vector<std::shared_ptr<base_reactor>>&, trigger&)>
        m_function;
  };

  template<typename F>
  function_reactor_builder::function_reactor_builder(F&& f)
      : m_function(std::forward<F>(f)) {}

  inline std::shared_ptr<base_reactor> function_reactor_builder::build(
      const std::vector<std::shared_ptr<base_reactor>>& parameters,
      trigger& t) const {
    return m_function(parameters, t);
  }

  inline std::shared_ptr<base_reactor> reactor_builder::build(
      trigger& t) const {
    return build({}, t);
  }
}

#endif
