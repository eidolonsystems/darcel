#ifndef DARCEL_FUNCTION_REACTOR_HPP
#define DARCEL_FUNCTION_REACTOR_HPP
#include <cassert>
#include <functional>
#include <optional>
#include <tuple>
#include <type_traits>
#include <utility>
#include "darcel/reactors/commit_reactor.hpp"
#include "darcel/reactors/reactor.hpp"
#include "darcel/reactors/reactor_unavailable_exception.hpp"
#include "darcel/reactors/reactors.hpp"
#include "darcel/utilities/maybe.hpp"

namespace darcel {

  /*! \brief Stores the result of a function used in a function reactor.
      \tparam T The result of the function.
   */
  template<typename T>
  struct function_evaluation {
    using type = T;
    std::optional<maybe<type>> m_value;
    base_reactor::update m_update;

    //! Constructs an uninitialized evaluation.
    function_evaluation();

    //! Constructs an evaluation resulting in a value and an EVAL.
    /*!
      \param value The value returned by the function.
    */
    function_evaluation(maybe<type> value);

    //! Constructs an evaluation resulting in a value and an EVAL.
    /*!
      \param value The value returned by the function.
    */
    function_evaluation(type value);

    //! Constructs an evaluation resulting in a value and an EVAL.
    /*!
      \param value The value returned by the function.
    */
    function_evaluation(std::optional<maybe<type>> value);

    //! Constructs an evaluation resulting in a value and an EVAL.
    /*!
      \param value The value returned by the function.
    */
    function_evaluation(std::optional<type> value);

    //! Constructs an evaluation resulting in a value and an update.
    /*!
      \param value The value returned by the function.
      \param update The type of update.
    */
    function_evaluation(maybe<type> value, base_reactor::update update);

    //! Constructs an evaluation resulting in a value and an update.
    /*!
      \param value The value returned by the function.
      \param update The type of update.
    */
    function_evaluation(type value, base_reactor::update update);

    //! Constructs an evaluation resulting in a value and an update.
    /*!
      \param value The value returned by the function.
      \param update The type of update.
    */
    function_evaluation(std::optional<maybe<type>> value,
      base_reactor::update update);

    //! Constructs an evaluation resulting in a value and an update.
    /*!
      \param value The value returned by the function.
      \param update The type of update.
    */
    function_evaluation(std::optional<type> value, base_reactor::update update);

    //! Constructs an evaluation resulting in just an update.
    /*!
      \param update The update.
    */
    function_evaluation(base_reactor::update update);
  };

namespace details {
  template<typename F, typename... P>
  struct invocation_type {
    using type = std::decay_t<
      decltype(std::declval<F>()(std::declval<P>()...))>;
  };

  template<typename F, typename... P>
  using invocation_type_t = typename invocation_type<F, P...>::type;

  template<typename T>
  struct function_reactor_type {
    using type = T;
  };

  template<typename T>
  struct function_reactor_type<std::optional<T>> {
    using type = T;
  };

  template<typename T>
  struct function_reactor_type<function_evaluation<T>> {
    using type = T;
  };

  struct function_eval {
    template<typename T>
    auto operator ()(const reactor<T>* reactor) const {
      return reactor->eval();
    }

    maybe<void> operator ()(const base_reactor* reactor) const {
      return {};
    }
  };

  template<typename T>
  struct function_update_eval {
    template<typename V, typename F, typename P>
    base_reactor::update operator ()(V& value, F& function, const P& p) const {
      auto evaluation = std::apply(
        [&] (const auto&... parameters) {
          return function_evaluation<T>(function(
            try_call(std::bind(function_eval(), &*parameters))...));
        }, p);
      if(evaluation.m_value.has_value()) {
        value = std::move(*evaluation.m_value);
      }
      return evaluation.m_update;
    }
  };

  template<>
  struct function_update_eval<void> {
    template<typename V, typename F, typename P>
    base_reactor::update operator ()(V& value, F& function, const P& p) const {
      std::apply(
        [&] (const auto&... parameters) {
          function(try_call(std::bind(function_eval(), &*parameters))...);
        }, p);
      return base_reactor::update::EVAL;
    }
  };

  inline void append_child(std::vector<base_reactor*>& children) {}

  template<typename P, typename... Tail>
  void append_child(std::vector<base_reactor*>& children, P&& parameter,
      Tail&&... tail) {
    children.push_back(&*parameter);
    append_child(children, std::forward<Tail>(tail)...);
  }

  template<typename... P>
  void initialize_children(std::vector<base_reactor*>& children,
      const std::tuple<P...>& parameters) {
    std::apply(
      [&] (auto&&... p) {
        append_child(children, p...);
      }, parameters);
  }
}

  /*! \brief A reactor that applies a function to its parameters.
      \tparam F The type of function to apply.
      \tparam P The type of parameters to apply the function to.
   */
  template<typename F, typename... P>
  class function_reactor final : public reactor<
      typename details::function_reactor_type<details::invocation_type_t<F,
      const maybe<reactor_type_t<P>>&...>>::type> {
    public:
      using type = typename reactor<
        typename details::function_reactor_type<details::invocation_type_t<
        F, const maybe<reactor_type_t<P>>&...>>::type>::type;

      //! The type of function to apply.
      using function = F;

      //! Constructs a function reactor.
      /*!
        \param function The function to apply.
        \param parameters The parameters to apply the <i>function</i> to.
      */
      template<typename FF, typename... PF>
      function_reactor(FF&& function, PF&&... parameters);

      base_reactor::update commit(int sequence) override;

      type eval() const override;

    private:
      function m_function;
      std::tuple<P...> m_parameters;
      std::optional<commit_reactor> m_commit_reactor;
      maybe<type> m_value;
      int m_sequence;
      base_reactor::update m_update;
      bool m_has_eval;

      base_reactor::update invoke();
  };

  //! Makes a function reactor.
  /*!
    \param function The function to apply.
    \param parameters The parameters to apply the <i>function</i> to.
  */
  template<typename F, typename... P>
  auto make_function_reactor(F&& function, P&&... parameters) {
    return std::make_shared<function_reactor<std::decay_t<F>,
      std::decay_t<P>...>>(std::forward<F>(function),
      std::forward<P>(parameters)...);
  }

  //! Makes a function evaluation.
  /*!
    \param value The value to return.
    \param update The state of the reactor.
  */
  template<typename T>
  auto make_function_evaluation(maybe<T> value, base_reactor::update update) {
    return function_evaluation<T>(std::move(value), update);
  }

  //! Makes a function evaluation.
  /*!
    \param value The value to return.
    \param update The state of the reactor.
  */
  template<typename T>
  auto make_function_evaluation(std::optional<maybe<T>> value,
      base_reactor::update update) {
    return function_evaluation<T>(std::move(value), update);
  }

  //! Makes a function evaluation.
  /*!
    \param value The value to return.
    \param update The state of the reactor.
  */
  template<typename T>
  auto make_function_evaluation(const T& value, base_reactor::update update) {
    return function_evaluation<T>(value, update);
  }

  template<typename T>
  function_evaluation<T>::function_evaluation()
      : m_update(base_reactor::update::NONE) {}

  template<typename T>
  function_evaluation<T>::function_evaluation(maybe<type> value)
      : m_value(std::move(value)),
        m_update(base_reactor::update::EVAL) {}

  template<typename T>
  function_evaluation<T>::function_evaluation(type value)
      : function_evaluation(maybe<type>(std::move(value))) {}

  template<typename T>
  function_evaluation<T>::function_evaluation(std::optional<maybe<type>> value)
      : m_value(std::move(value)) {
    if(m_value.has_value()) {
      m_update = base_reactor::update::EVAL;
    } else {
      m_update = base_reactor::update::NONE;
    }
  }

  template<typename T>
  function_evaluation<T>::function_evaluation(std::optional<type> value)
      : function_evaluation(std::optional<maybe<type>>(std::move(value))) {}

  template<typename T>
  function_evaluation<T>::function_evaluation(maybe<type> value,
      base_reactor::update update)
      : m_value(std::move(value)),
        m_update(update) {
    combine(m_update, base_reactor::update::EVAL);
  }

  template<typename T>
  function_evaluation<T>::function_evaluation(type value,
      base_reactor::update update)
      : m_value(std::move(value)),
        m_update(update) {
    combine(m_update, base_reactor::update::EVAL);
  }

  template<typename T>
  function_evaluation<T>::function_evaluation(std::optional<maybe<type>> value,
      base_reactor::update update)
      : m_value(std::move(value)),
        m_update(update) {
    if(m_value.has_value()) {
      combine(m_update, base_reactor::update::EVAL);
    } else {
      assert(m_update != base_reactor::update::EVAL);
    }
  }

  template<typename T>
  function_evaluation<T>::function_evaluation(std::optional<type> value,
      base_reactor::update update)
      : function_evaluation(std::optional<maybe<type>>(
          maybe<type>(std::move(value))), update) {}

  template<typename T>
  function_evaluation<T>::function_evaluation(base_reactor::update update)
      : m_update(update) {
    assert(m_update != base_reactor::update::EVAL);
  }

  template<typename F, typename... P>
  template<typename FF, typename... PF>
  function_reactor<F, P...>::function_reactor(FF&& function, PF&&... parameters)
      : m_function(std::forward<FF>(function)),
        m_parameters(std::forward<PF>(parameters)...),
        m_value(std::make_exception_ptr(reactor_unavailable_exception())),
        m_sequence(-1),
        m_update(base_reactor::update::NONE),
        m_has_eval(false) {
    std::vector<base_reactor*> children;
    details::initialize_children(children, m_parameters);
    m_commit_reactor.emplace(std::move(children));
  }

  template<typename F, typename... P>
  base_reactor::update function_reactor<F, P...>::commit(int sequence) {
    if(is_complete(m_update) || m_sequence == sequence) {
      return m_update;
    }
    m_update = m_commit_reactor->commit(sequence);
    if(has_eval(m_update)) {
      auto invocation = invoke();
      if(invocation == base_reactor::update::NONE) {
        if(is_complete(m_update)) {
          if(m_has_eval) {
            m_update = base_reactor::update::COMPLETE_EVAL;
          } else {
            m_update = base_reactor::update::COMPLETE_EMPTY;
          }
        } else {
          m_update = base_reactor::update::NONE;
        }
      } else if(is_complete(invocation)) {
        if(m_has_eval || has_eval(invocation)) {
          m_update = base_reactor::update::COMPLETE_EVAL;
        } else {
          m_update = base_reactor::update::COMPLETE_EMPTY;
        }
      }
    }
    m_sequence = sequence;
    m_has_eval |= has_eval(m_update);
    return m_update;
  }

  template<typename F, typename... P>
  typename function_reactor<F, P...>::type
      function_reactor<F, P...>::eval() const {
    return m_value.get();
  }

  template<typename F, typename... P>
  base_reactor::update function_reactor<F, P...>::invoke() {
    try {
      return details::function_update_eval<type>()(m_value, m_function,
        m_parameters);
    } catch(const std::exception&) {
      m_value = std::current_exception();
      return base_reactor::update::EVAL;
    }
  }
}

#endif
