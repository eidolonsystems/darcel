#ifndef DARCEL_REACTOR_HPP
#define DARCEL_REACTOR_HPP
#include "darcel/reactors/base_reactor.hpp"
#include "darcel/reactors/reactors.hpp"

namespace darcel {

  /*! \brief The base class of a reactor.
      \tparam T The type this reactor evaluates to.
   */
  template<typename T>
  class reactor : public base_reactor {
    public:

      //! The type this reactor evaluates to.
      using type = T;

      const std::type_info& get_type() const override final;

      //! Evaluates this reactor.
      virtual type eval() const = 0;

    protected:

      //! Default constructor.
      reactor() = default;
  };

  template<typename T>
  const std::type_info& reactor<T>::get_type() const {
    return typeid(type);
  }
}

#endif
