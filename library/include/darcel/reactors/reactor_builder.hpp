#ifndef DARCEL_REACTOR_BUILDER_HPP
#define DARCEL_REACTOR_BUILDER_HPP
#include <memory>
#include <vector>
#include "darcel/reactors/reactors.hpp"

namespace darcel {

  //! Builds a reactor composed of a list of sub-reactors.
  class reactor_builder {
    public:
      virtual ~reactor_builder() = default;

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
}

#endif
