#ifndef DARCEL_REACTORS_HPP
#define DARCEL_REACTORS_HPP

namespace darcel {
  class BaseProxyReactor;
  class BaseReactor;
  template<typename T> class ChainReactor;
  class CommitReactor;
  template<typename T> class ConstantReactor;
  template<typename T> class FoldReactor;
  template<typename T> class FoldParameterReactor;
  template<typename F, typename... P> class FunctionReactor;
  template<typename T> class NoneReactor;
  class PerpetualReactor;
  template<typename T> class ProxyReactor;
  template<typename T> class QueueReactor;
  template<typename T> class Reactor;
  class ReactorBuilder;
  class ReactorError;
  class ReactorException;
  class ReactorExecutor;
  class ReactorTranslator;
  class ReactorUnavailableException;
  class Trigger;
  class UpdateReactor;
}

#endif
