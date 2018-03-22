#ifndef DARCEL_OSTREAM_REACTOR_HPP
#define DARCEL_OSTREAM_REACTOR_HPP
#include <memory>
#include <ostream>
#include "darcel/reactors/reactor.hpp"
#include "darcel/reactors/reactor_builder.hpp"
#include "darcel/reactors/reactors.hpp"

namespace darcel {

  //! Prints the values produced by its parameter.
  /*!
    \param out The stream to print to.
    \param source The reactor producing the values to print.
  */
  template<typename T>
  auto make_ostream_reactor(std::ostream& out,
      std::shared_ptr<reactor<T>> source) {
    return make_function_reactor(
      [out = &out] (const T& value) -> decltype(auto) {
        *out << value << "\n";
        return value;
      }, std::move(source));
  }

  //! Builds an ostream reactor.
  template<typename T>
  class ostream_reactor_builder : public reactor_builder {
    public:

      //! Constructs an ostream builder.
      /*!
        \param out The ostream to print to.
      */
      ostream_reactor_builder(std::ostream& out);

      std::shared_ptr<base_reactor> build(
        const std::vector<std::shared_ptr<base_reactor>>& parameters,
        trigger& t) const override final;

    private:
      std::ostream* m_out;
  };

  template<typename T>
  ostream_reactor_builder<T>::ostream_reactor_builder(std::ostream& out)
      : m_out(&out) {}

  template<typename T>
  std::shared_ptr<base_reactor> ostream_reactor_builder<T>::build(
      const std::vector<std::shared_ptr<base_reactor>>& parameters,
      trigger& t) const {
    return make_ostream_reactor(*m_out,
      std::static_pointer_cast<reactor<T>>(parameters.front()));
  }
}

#endif
