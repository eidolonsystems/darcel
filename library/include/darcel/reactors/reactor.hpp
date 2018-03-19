#ifndef DARCEL_REACTOR_HPP
#define DARCEL_REACTOR_HPP
#include <memory>
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

  /*! \brief Returns a reactor's evaluation type.
      \tparam T The reactor to evaluate.
   */
  template<typename T>
  struct reactor_type {
    using type = typename T::type;
  };

  template<typename T>
  struct reactor_type<std::shared_ptr<T>> {
    using type = typename reactor_type<T>::type;
  };

  template<typename T>
  using reactor_type_t = typename reactor_type<T>::type;

  template<typename T>
  const std::type_info& reactor<T>::get_type() const {
    return typeid(type);
  }
}

#endif
