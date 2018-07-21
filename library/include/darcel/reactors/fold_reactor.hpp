#ifndef DARCEL_FOLD_REACTOR_HPP
#define DARCEL_FOLD_REACTOR_HPP
#include <memory>
#include <optional>
#include <utility>
#include "darcel/reactors/constant_reactor.hpp"
#include "darcel/reactors/reactor.hpp"
#include "darcel/reactors/reactor_builder.hpp"
#include "darcel/reactors/reactor_unavailable_exception.hpp"
#include "darcel/reactors/reactors.hpp"
#include "darcel/utilities/maybe.hpp"

namespace darcel {
namespace details {
  template<typename T>
  struct fold_parameter_type {
    using type = std::shared_ptr<FoldParameterReactor<reactor_type_t<
      decltype(lift(std::forward<T>(std::declval<T&&>())))>>>;
  };

  template<typename T>
  using fold_parameter_t = typename fold_parameter_type<T>::type;
}

  /*! \brief Placeholder used as parameters to the fold reactor.
      \tparam T The type of placeholder to evaluate to.
   */
  template<typename T>
  class FoldParameterReactor final : public Reactor<T> {
    public:
      using Type = typename Reactor<T>::Type;

      //! Constructs a fold parameter reactor.
      FoldParameterReactor();

      BaseReactor::Update commit(int sequence) override;

      Type eval() const override;

    private:
      template<typename> friend class FoldReactor;
      maybe<Type> m_value;
      int m_sequence;
      maybe<Type> m_next_value;
      int m_next_sequence;

      void set(maybe<Type> value, int sequence);
  };

  /*! \brief Folds the values produced by a reactor.
      \tparam T The type of value to evaluate.
   */
  template<typename T>
  class FoldReactor final : public Reactor<T> {
    public:
      using Type = typename Reactor<T>::Type;

      //! Constructs a fold reactor.
      /*!
        \param evaluation The reactor to evaluate.
        \param left The parameter used for the left hand side of the evaluation.
        \param right The parameter used for the right hand side of the
               evaluation.
        \param producer The reactor producing the values to fold.
      */
      FoldReactor(std::shared_ptr<Reactor<Type>> evaluation,
        std::shared_ptr<FoldParameterReactor<Type>> left,
        std::shared_ptr<FoldParameterReactor<Type>> right,
        std::shared_ptr<Reactor<Type>> producer);

      BaseReactor::Update commit(int sequence) override;

      Type eval() const override;

    private:
      std::shared_ptr<Reactor<Type>> m_evaluation;
      std::shared_ptr<FoldParameterReactor<Type>> m_left;
      std::shared_ptr<FoldParameterReactor<Type>> m_right;
      std::shared_ptr<Reactor<Type>> m_producer;
      maybe<Type> m_value;
      std::optional<maybe<Type>> m_previous_value;
      int m_sequence;
      BaseReactor::Update m_update;
  };

  //! Makes a fold parameter.
  template<typename T>
  auto make_fold_parameter() {
    return std::make_shared<FoldParameterReactor<T>>();
  }

  //! Makes a fold reactor.
  /*!
    \param evaluation The reactor to evaluate.
    \param left The parameter used for the left hand side of the evaluation.
    \param right The parameter used for the right hand side of the evaluation.
    \param producer The reactor producing the values to fold.
  */
  template<typename T>
  auto make_fold_reactor(std::shared_ptr<Reactor<T>> evaluation,
      std::shared_ptr<FoldParameterReactor<T>> left,
      std::shared_ptr<FoldParameterReactor<T>> right,
      std::shared_ptr<Reactor<T>> producer) {
    return std::make_shared<FoldReactor<T>>(std::move(evaluation),
      std::move(left), std::move(right), std::move(producer));
  }

  //! Makes a fold reactor.
  /*!
    \param evaluation The reactor to evaluate.
    \param left The parameter used for the left hand side of the evaluation.
    \param right The parameter used for the right hand side of the evaluation.
    \param producer The reactor producing the values to fold.
  */
  template<typename E, typename P>
  auto fold(E&& evaluation, details::fold_parameter_t<E> left,
      details::fold_parameter_t<E> right, P&& producer) {
    auto producer_reactor = lift(std::forward<P>(producer));
    return make_fold_reactor(lift(std::forward<E>(evaluation)), std::move(left),
      std::move(right), lift(std::forward<P>(producer)));
  }

  //! Builds a fold reactor.
  template<typename T>
  auto make_fold_reactor_builder() {
    return std::make_unique<FunctionReactorBuilder>(
      [] (auto& parameters, auto& t) {
        auto left = make_fold_parameter<T>();
        auto right = make_fold_parameter<T>();
        auto f = std::static_pointer_cast<Reactor<T>>(parameters[0]->build(
          {make_passthrough_reactor_builder(left),
           make_passthrough_reactor_builder(right)}, t));
        auto p = std::static_pointer_cast<Reactor<T>>(parameters[1]->build(t));
        return make_fold_reactor(std::move(f), std::move(left),
          std::move(right), std::move(p));
      });
  }

  //! Invokes a reactor's eval method, wrapping any thrown exception in a maybe.
  /*!
    \param r The reactor to eval.
    \return The result of the eval, capturing any thrown exception.
  */
  template<typename R>
  auto try_eval(const R& r) {
    return try_call(
      [&] {
        return r.eval();
      });
  }

  template<typename T>
  FoldParameterReactor<T>::FoldParameterReactor()
      : m_value(std::make_exception_ptr(ReactorUnavailableException())),
        m_sequence(-1),
        m_next_value(std::make_exception_ptr(ReactorUnavailableException())),
        m_next_sequence(-1) {}

  template<typename T>
  BaseReactor::Update FoldParameterReactor<T>::commit(int sequence) {
    if(sequence == m_sequence) {
      return BaseReactor::Update::EVAL;
    } else if(sequence == m_next_sequence) {
      m_value = std::move(m_next_value);
      m_sequence = sequence;
      m_next_sequence = -1;
      return BaseReactor::Update::EVAL;
    }
    return BaseReactor::Update::NONE;
  }

  template<typename T>
  typename FoldParameterReactor<T>::Type
      FoldParameterReactor<T>::eval() const {
    return m_value.get();
  }

  template<typename T>
  void FoldParameterReactor<T>::set(maybe<Type> value, int sequence) {
    m_next_value = std::move(value);
    m_next_sequence = sequence;
  }

  template<typename T>
  FoldReactor<T>::FoldReactor(std::shared_ptr<Reactor<Type>> evaluation,
      std::shared_ptr<FoldParameterReactor<Type>> left,
      std::shared_ptr<FoldParameterReactor<Type>> right,
      std::shared_ptr<Reactor<Type>> producer)
      : m_evaluation(std::move(evaluation)),
        m_left(std::move(left)),
        m_right(std::move(right)),
        m_producer(std::move(producer)),
        m_value(std::make_exception_ptr(ReactorUnavailableException())),
        m_sequence(-1),
        m_update(BaseReactor::Update::NONE) {}

  template<typename T>
  BaseReactor::Update FoldReactor<T>::commit(int sequence) {
    if(m_sequence == sequence || is_complete(m_update)) {
      return m_update;
    }
    auto producer_update = m_producer->commit(sequence);
    if(producer_update == BaseReactor::Update::NONE) {
      return BaseReactor::Update::NONE;
    } else if(has_eval(producer_update)) {
      if(!m_previous_value.has_value()) {
        if(is_complete(producer_update)) {
          m_update = BaseReactor::Update::COMPLETE_EMPTY;
          return BaseReactor::Update::COMPLETE_EMPTY;
        }
        m_previous_value = try_eval(*m_producer);
        m_sequence = sequence;
        m_update = BaseReactor::Update::NONE;
        return BaseReactor::Update::NONE;
      }
      m_left->set(std::move(*m_previous_value), sequence);
      m_right->set(try_eval(*m_producer), sequence);
      m_update = m_evaluation->commit(sequence);
      if(has_eval(m_update)) {
        m_value = try_eval(*m_evaluation);
        m_previous_value = m_value;
      }
    }
    if(is_complete(producer_update)) {
      combine(m_update, BaseReactor::Update::COMPLETE);
    }
    m_sequence = sequence;
    return m_update;
  }

  template<typename T>
  typename FoldReactor<T>::Type FoldReactor<T>::eval() const {
    return m_value.get();
  }
}

#endif
