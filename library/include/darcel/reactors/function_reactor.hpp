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
  struct FunctionEvaluation {
    using Type = T;
    std::optional<maybe<Type>> m_value;
    BaseReactor::Update m_update;

    //! Constructs an uninitialized evaluation.
    FunctionEvaluation();

    //! Constructs an evaluation resulting in a value and an EVAL.
    /*!
      \param value The value returned by the function.
    */
    FunctionEvaluation(maybe<Type> value);

    //! Constructs an evaluation resulting in a value and an EVAL.
    /*!
      \param value The value returned by the function.
    */
    FunctionEvaluation(Type value);

    //! Constructs an evaluation resulting in a value and an EVAL.
    /*!
      \param value The value returned by the function.
    */
    FunctionEvaluation(std::optional<maybe<Type>> value);

    //! Constructs an evaluation resulting in a value and an EVAL.
    /*!
      \param value The value returned by the function.
    */
    FunctionEvaluation(std::optional<Type> value);

    //! Constructs an evaluation resulting in a value and an update.
    /*!
      \param value The value returned by the function.
      \param update The type of update.
    */
    FunctionEvaluation(maybe<Type> value, BaseReactor::Update update);

    //! Constructs an evaluation resulting in a value and an update.
    /*!
      \param value The value returned by the function.
      \param update The type of update.
    */
    FunctionEvaluation(Type value, BaseReactor::Update update);

    //! Constructs an evaluation resulting in a value and an update.
    /*!
      \param value The value returned by the function.
      \param update The type of update.
    */
    FunctionEvaluation(std::optional<maybe<Type>> value,
      BaseReactor::Update update);

    //! Constructs an evaluation resulting in a value and an update.
    /*!
      \param value The value returned by the function.
      \param update The type of update.
    */
    FunctionEvaluation(std::optional<Type> value, BaseReactor::Update update);

    //! Constructs an evaluation resulting in just an update.
    /*!
      \param update The update.
    */
    FunctionEvaluation(BaseReactor::Update update);
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
  struct function_reactor_type<FunctionEvaluation<T>> {
    using type = T;
  };

  struct FunctionEval {
    template<typename T>
    auto operator ()(const Reactor<T>* reactor) const {
      return reactor->eval();
    }

    maybe<void> operator ()(const BaseReactor* reactor) const {
      return {};
    }
  };

  template<typename T>
  struct FunctionUpdateEval {
    template<typename V, typename F, typename P>
    BaseReactor::Update operator ()(V& value, F& function, const P& p) const {
      auto evaluation = std::apply(
        [&] (const auto&... parameters) {
          return FunctionEvaluation<T>(function(
            try_call(std::bind(FunctionEval(), &*parameters))...));
        }, p);
      if(evaluation.m_value.has_value()) {
        value = std::move(*evaluation.m_value);
      }
      return evaluation.m_update;
    }
  };

  template<>
  struct FunctionUpdateEval<void> {
    template<typename V, typename F, typename P>
    BaseReactor::Update operator ()(V& value, F& function, const P& p) const {
      std::apply(
        [&] (const auto&... parameters) {
          function(try_call(std::bind(FunctionEval(), &*parameters))...);
        }, p);
      return BaseReactor::Update::EVAL;
    }
  };

  inline void append_child(std::vector<BaseReactor*>& children) {}

  template<typename P, typename... Tail>
  void append_child(std::vector<BaseReactor*>& children, P&& parameter,
      Tail&&... tail) {
    children.push_back(&*parameter);
    append_child(children, std::forward<Tail>(tail)...);
  }

  template<typename... P>
  void initialize_children(std::vector<BaseReactor*>& children,
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
  class FunctionReactor final : public Reactor<
      typename details::function_reactor_type<details::invocation_type_t<F,
      const maybe<reactor_type_t<P>>&...>>::type> {
    public:
      using type = typename Reactor<
        typename details::function_reactor_type<details::invocation_type_t<
        F, const maybe<reactor_type_t<P>>&...>>::type>::Type;

      //! The type of function to apply.
      using Function = F;

      //! Constructs a function reactor.
      /*!
        \param function The function to apply.
        \param parameters The parameters to apply the <i>function</i> to.
      */
      template<typename FF, typename... PF>
      FunctionReactor(FF&& function, PF&&... parameters);

      BaseReactor::Update commit(int sequence) override;

      type eval() const override;

    private:
      Function m_function;
      std::tuple<P...> m_parameters;
      std::optional<CommitReactor> m_commit_reactor;
      maybe<type> m_value;
      int m_sequence;
      BaseReactor::Update m_update;
      bool m_has_eval;

      BaseReactor::Update invoke();
  };

  //! Makes a function reactor.
  /*!
    \param function The function to apply.
    \param parameters The parameters to apply the <i>function</i> to.
  */
  template<typename F, typename... P>
  auto make_function_reactor(F&& function, P&&... parameters) {
    return std::make_shared<FunctionReactor<std::decay_t<F>,
      std::decay_t<P>...>>(std::forward<F>(function),
      std::forward<P>(parameters)...);
  }

  //! Makes a function evaluation.
  /*!
    \param value The value to return.
    \param update The state of the reactor.
  */
  template<typename T>
  auto make_function_evaluation(maybe<T> value, BaseReactor::Update update) {
    return FunctionEvaluation<T>(std::move(value), update);
  }

  //! Makes a function evaluation.
  /*!
    \param value The value to return.
    \param update The state of the reactor.
  */
  template<typename T>
  auto make_function_evaluation(std::optional<maybe<T>> value,
      BaseReactor::Update update) {
    return FunctionEvaluation<T>(std::move(value), update);
  }

  //! Makes a function evaluation.
  /*!
    \param value The value to return.
    \param update The state of the reactor.
  */
  template<typename T>
  auto make_function_evaluation(const T& value, BaseReactor::Update update) {
    return FunctionEvaluation<T>(value, update);
  }

  template<typename T>
  FunctionEvaluation<T>::FunctionEvaluation()
      : m_update(BaseReactor::Update::NONE) {}

  template<typename T>
  FunctionEvaluation<T>::FunctionEvaluation(maybe<Type> value)
      : m_value(std::move(value)),
        m_update(BaseReactor::Update::EVAL) {}

  template<typename T>
  FunctionEvaluation<T>::FunctionEvaluation(Type value)
      : FunctionEvaluation(maybe<Type>(std::move(value))) {}

  template<typename T>
  FunctionEvaluation<T>::FunctionEvaluation(std::optional<maybe<Type>> value)
      : m_value(std::move(value)) {
    if(m_value.has_value()) {
      m_update = BaseReactor::Update::EVAL;
    } else {
      m_update = BaseReactor::Update::NONE;
    }
  }

  template<typename T>
  FunctionEvaluation<T>::FunctionEvaluation(std::optional<Type> value)
      : FunctionEvaluation(std::optional<maybe<Type>>(std::move(value))) {}

  template<typename T>
  FunctionEvaluation<T>::FunctionEvaluation(maybe<Type> value,
      BaseReactor::Update update)
      : m_value(std::move(value)),
        m_update(update) {
    combine(m_update, BaseReactor::Update::EVAL);
  }

  template<typename T>
  FunctionEvaluation<T>::FunctionEvaluation(Type value,
      BaseReactor::Update update)
      : m_value(std::move(value)),
        m_update(update) {
    combine(m_update, BaseReactor::Update::EVAL);
  }

  template<typename T>
  FunctionEvaluation<T>::FunctionEvaluation(std::optional<maybe<Type>> value,
      BaseReactor::Update update)
      : m_value(std::move(value)),
        m_update(update) {
    if(m_value.has_value()) {
      combine(m_update, BaseReactor::Update::EVAL);
    } else {
      assert(m_update != BaseReactor::Update::EVAL);
    }
  }

  template<typename T>
  FunctionEvaluation<T>::FunctionEvaluation(std::optional<Type> value,
      BaseReactor::Update update)
      : FunctionEvaluation(std::optional<maybe<Type>>(
          maybe<Type>(std::move(value))), update) {}

  template<typename T>
  FunctionEvaluation<T>::FunctionEvaluation(BaseReactor::Update update)
      : m_update(update) {
    assert(m_update != BaseReactor::Update::EVAL);
  }

  template<typename F, typename... P>
  template<typename FF, typename... PF>
  FunctionReactor<F, P...>::FunctionReactor(FF&& function, PF&&... parameters)
      : m_function(std::forward<FF>(function)),
        m_parameters(std::forward<PF>(parameters)...),
        m_value(std::make_exception_ptr(ReactorUnavailableException())),
        m_sequence(-1),
        m_update(BaseReactor::Update::NONE),
        m_has_eval(false) {
    std::vector<BaseReactor*> children;
    details::initialize_children(children, m_parameters);
    m_commit_reactor.emplace(std::move(children));
  }

  template<typename F, typename... P>
  BaseReactor::Update FunctionReactor<F, P...>::commit(int sequence) {
    if(is_complete(m_update) || m_sequence == sequence) {
      return m_update;
    }
    m_update = m_commit_reactor->commit(sequence);
    if(has_eval(m_update)) {
      auto invocation = invoke();
      if(invocation == BaseReactor::Update::NONE) {
        if(is_complete(m_update)) {
          if(m_has_eval) {
            m_update = BaseReactor::Update::COMPLETE_EVAL;
          } else {
            m_update = BaseReactor::Update::COMPLETE_EMPTY;
          }
        } else {
          m_update = BaseReactor::Update::NONE;
        }
      } else if(is_complete(invocation)) {
        if(m_has_eval || has_eval(invocation)) {
          m_update = BaseReactor::Update::COMPLETE_EVAL;
        } else {
          m_update = BaseReactor::Update::COMPLETE_EMPTY;
        }
      }
    }
    m_sequence = sequence;
    m_has_eval |= has_eval(m_update);
    return m_update;
  }

  template<typename F, typename... P>
  typename FunctionReactor<F, P...>::type
      FunctionReactor<F, P...>::eval() const {
    return m_value.get();
  }

  template<typename F, typename... P>
  BaseReactor::Update FunctionReactor<F, P...>::invoke() {
    try {
      return details::FunctionUpdateEval<type>()(m_value, m_function,
        m_parameters);
    } catch(const std::exception&) {
      m_value = std::current_exception();
      return BaseReactor::Update::EVAL;
    }
  }
}

#endif
