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
  class Reactor : public BaseReactor {
    public:

      //! The type this reactor evaluates to.
      using Type = T;

      const std::type_info& get_type() const override final;

      //! Evaluates this reactor.
      virtual Type eval() const = 0;

    protected:

      //! Default constructor.
      Reactor() = default;
  };

  /*! \brief Returns a reactor's evaluation type.
      \tparam T The reactor to evaluate.
   */
  template<typename T>
  struct reactor_type {
    using type = typename T::Type;
  };

  template<typename T>
  struct reactor_type<std::shared_ptr<T>> {
    using type = typename reactor_type<T>::type;
  };

  template<typename T>
  using reactor_type_t = typename reactor_type<T>::type;

  template<typename T>
  const std::type_info& Reactor<T>::get_type() const {
    return typeid(Type);
  }
}

#endif
