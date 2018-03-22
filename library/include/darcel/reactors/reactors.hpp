#ifndef DARCEL_REACTORS_HPP
#define DARCEL_REACTORS_HPP

namespace darcel {
  class base_reactor;
  template<typename T> class chain_reactor;
  template<typename T> class chain_reactor_builder;
  class commit_reactor;
  template<typename T> class constant_reactor;
  template<typename F, typename... P> class function_reactor;
  template<typename T> class none_reactor;
  template<typename T> class ostream_reactor_builder;
  template<typename T> class reactor;
  class reactor_builder;
  class reactor_error;
  class reactor_exception;
  class reactor_executor;
  class reactor_translator;
  class reactor_unavailable_exception;
  class trigger;
}

#endif
