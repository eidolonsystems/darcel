#ifndef DARCEL_PROXY_REACTOR_HPP
#define DARCEL_PROXY_REACTOR_HPP
#include <memory>
#include "darcel/reactors/reactor.hpp"
#include "darcel/reactors/reactors.hpp"

namespace darcel {

  //! The untyped base class for a proxy reactor.
  class base_proxy_reactor {
    public:
      virtual ~base_proxy_reactor() = default;

      //! The reactor to proxy calls to.
      virtual void set_source(std::shared_ptr<base_reactor> source) = 0;

    protected:
      base_proxy_reactor() = default;

    private:
      base_proxy_reactor(const base_proxy_reactor&) = delete;
      base_proxy_reactor& operator =(const base_proxy_reactor&) = delete;
  };

  //! A reactor that delegates all operations to another reactor.
  template<typename T>
  class proxy_reactor : public reactor<T>, public base_proxy_reactor {
    public:
      using type = typename reactor<T>::type;

      //! Constructs a proxy reactor.
      proxy_reactor() = default;

      base_reactor::update commit(int sequence) override final;

      type eval() const override final;

      void set_source(std::shared_ptr<base_reactor> source) override final;

    private:
      std::shared_ptr<reactor<T>> m_source;
  };

  //! Makes a proxy reactor.
  template<typename T>
  auto make_proxy_reactor() {
    return std::make_shared<proxy_reactor<T>>();
  };

  //! Makes a proxy reactor.
  template<typename T>
  auto proxy() {
    return make_proxy_reactor<T>();
  }

  //! Makes a proxy reactor builder.
  template<typename T>
  auto make_proxy_reactor_builder() {
    return std::make_unique<function_reactor_builder>(
      [] (auto& parameters, auto& t) {
        return std::make_shared<proxy_reactor<T>>();
      });
  }

  template<typename T>
  base_reactor::update proxy_reactor<T>::commit(int sequence) {
    return m_source->commit(sequence);
  }

  template<typename T>
  typename proxy_reactor<T>::type proxy_reactor<T>::eval() const {
    return m_source->eval();
  }

  template<typename T>
  void proxy_reactor<T>::set_source(std::shared_ptr<base_reactor> source) {
    m_source = std::static_pointer_cast<reactor<T>>(source);
  }
}

#endif
