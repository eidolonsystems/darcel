#ifndef DARCEL_PROXY_REACTOR_HPP
#define DARCEL_PROXY_REACTOR_HPP
#include <memory>
#include "darcel/reactors/reactor.hpp"
#include "darcel/reactors/reactors.hpp"

namespace darcel {

  //! The untyped base class for a proxy reactor.
  class BaseProxyReactor {
    public:
      virtual ~BaseProxyReactor() = default;

      //! The reactor to proxy calls to.
      virtual void set_source(std::shared_ptr<BaseReactor> source) = 0;

    protected:
      BaseProxyReactor() = default;

    private:
      BaseProxyReactor(const BaseProxyReactor&) = delete;
      BaseProxyReactor& operator =(const BaseProxyReactor&) = delete;
  };

  //! A reactor that delegates all operations to another reactor.
  template<typename T>
  class ProxyReactor final : public Reactor<T>, public BaseProxyReactor {
    public:
      using Type = typename Reactor<T>::Type;

      //! Constructs a proxy reactor.
      ProxyReactor() = default;

      BaseReactor::Update commit(int sequence) override;

      Type eval() const override;

      void set_source(std::shared_ptr<BaseReactor> source) override;

    private:
      std::shared_ptr<Reactor<T>> m_source;
  };

  //! Makes a proxy reactor.
  template<typename T>
  auto make_proxy_reactor() {
    return std::make_shared<ProxyReactor<T>>();
  };

  //! Makes a proxy reactor.
  template<typename T>
  auto proxy() {
    return make_proxy_reactor<T>();
  }

  //! Makes a proxy reactor builder.
  template<typename T>
  auto make_proxy_reactor_builder() {
    return std::make_unique<FunctionReactorBuilder>(
      [] (auto& parameters, auto& t) {
        return std::make_shared<ProxyReactor<T>>();
      });
  }

  template<typename T>
  BaseReactor::Update ProxyReactor<T>::commit(int sequence) {
    return m_source->commit(sequence);
  }

  template<typename T>
  typename ProxyReactor<T>::Type ProxyReactor<T>::eval() const {
    return m_source->eval();
  }

  template<typename T>
  void ProxyReactor<T>::set_source(std::shared_ptr<BaseReactor> source) {
    m_source = std::static_pointer_cast<Reactor<T>>(source);
  }
}

#endif
