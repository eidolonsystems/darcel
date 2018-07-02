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
    using type = std::shared_ptr<fold_parameter_reactor<reactor_type_t<
      decltype(lift(std::forward<T>(std::declval<T&&>())))>>>;
  };

  template<typename T>
  using fold_parameter_t = typename fold_parameter_type<T>::type;
}

  /*! \brief Placeholder used as parameters to the fold reactor.
      \tparam T The type of placeholder to evaluate to.
   */
  template<typename T>
  class fold_parameter_reactor final : public reactor<T> {
    public:
      using type = typename reactor<T>::type;

      //! Constructs a fold parameter reactor.
      fold_parameter_reactor();

      base_reactor::update commit(int sequence) override;

      type eval() const override;

    private:
      template<typename> friend class fold_reactor;
      maybe<type> m_value;
      int m_sequence;
      maybe<type> m_next_value;
      int m_next_sequence;

      void set(maybe<type> value, int sequence);
  };

  /*! \brief Folds the values produced by a reactor.
      \tparam T The type of value to evaluate.
   */
  template<typename T>
  class fold_reactor final : public reactor<T> {
    public:
      using type = typename reactor<T>::type;

      //! Constructs a fold reactor.
      /*!
        \param evaluation The reactor to evaluate.
        \param left The parameter used for the left hand side of the evaluation.
        \param right The parameter used for the right hand side of the
               evaluation.
        \param producer The reactor producing the values to fold.
      */
      fold_reactor(std::shared_ptr<reactor<type>> evaluation,
        std::shared_ptr<fold_parameter_reactor<type>> left,
        std::shared_ptr<fold_parameter_reactor<type>> right,
        std::shared_ptr<reactor<type>> producer);

      base_reactor::update commit(int sequence) override;

      type eval() const override;

    private:
      std::shared_ptr<reactor<type>> m_evaluation;
      std::shared_ptr<fold_parameter_reactor<type>> m_left;
      std::shared_ptr<fold_parameter_reactor<type>> m_right;
      std::shared_ptr<reactor<type>> m_producer;
      maybe<type> m_value;
      std::optional<maybe<type>> m_previous_value;
      int m_sequence;
      base_reactor::update m_update;
  };

  //! Makes a fold parameter.
  template<typename T>
  auto make_fold_parameter() {
    return std::make_shared<fold_parameter_reactor<T>>();
  }

  //! Makes a fold reactor.
  /*!
    \param evaluation The reactor to evaluate.
    \param left The parameter used for the left hand side of the evaluation.
    \param right The parameter used for the right hand side of the evaluation.
    \param producer The reactor producing the values to fold.
  */
  template<typename T>
  auto make_fold_reactor(std::shared_ptr<reactor<T>> evaluation,
      std::shared_ptr<fold_parameter_reactor<T>> left,
      std::shared_ptr<fold_parameter_reactor<T>> right,
      std::shared_ptr<reactor<T>> producer) {
    return std::make_shared<fold_reactor<T>>(std::move(evaluation),
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
    return std::make_unique<function_reactor_builder>(
      [] (auto& parameters, auto& t) {
        auto left = make_fold_parameter<T>();
        auto right = make_fold_parameter<T>();
        auto f = std::static_pointer_cast<reactor<T>>(parameters[0]->build(
          {make_passthrough_reactor_builder(left),
           make_passthrough_reactor_builder(right)}, t));
        auto p = std::static_pointer_cast<reactor<T>>(parameters[1]->build(t));
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
  fold_parameter_reactor<T>::fold_parameter_reactor()
      : m_value(std::make_exception_ptr(reactor_unavailable_exception())),
        m_sequence(-1),
        m_next_value(std::make_exception_ptr(reactor_unavailable_exception())),
        m_next_sequence(-1) {}

  template<typename T>
  base_reactor::update fold_parameter_reactor<T>::commit(int sequence) {
    if(sequence == m_sequence) {
      return base_reactor::update::EVAL;
    } else if(sequence == m_next_sequence) {
      m_value = std::move(m_next_value);
      m_sequence = sequence;
      m_next_sequence = -1;
      return base_reactor::update::EVAL;
    }
    return base_reactor::update::NONE;
  }

  template<typename T>
  typename fold_parameter_reactor<T>::type
      fold_parameter_reactor<T>::eval() const {
    return m_value.get();
  }

  template<typename T>
  void fold_parameter_reactor<T>::set(maybe<type> value, int sequence) {
    m_next_value = std::move(value);
    m_next_sequence = sequence;
  }

  template<typename T>
  fold_reactor<T>::fold_reactor(std::shared_ptr<reactor<type>> evaluation,
      std::shared_ptr<fold_parameter_reactor<type>> left,
      std::shared_ptr<fold_parameter_reactor<type>> right,
      std::shared_ptr<reactor<type>> producer)
      : m_evaluation(std::move(evaluation)),
        m_left(std::move(left)),
        m_right(std::move(right)),
        m_producer(std::move(producer)),
        m_value(std::make_exception_ptr(reactor_unavailable_exception())),
        m_sequence(-1),
        m_update(base_reactor::update::NONE) {}

  template<typename T>
  base_reactor::update fold_reactor<T>::commit(int sequence) {
    if(m_sequence == sequence || is_complete(m_update)) {
      return m_update;
    }
    auto producer_update = m_producer->commit(sequence);
    if(producer_update == base_reactor::update::NONE) {
      return base_reactor::update::NONE;
    } else if(has_eval(producer_update)) {
      if(!m_previous_value.has_value()) {
        if(is_complete(producer_update)) {
          m_update = base_reactor::update::COMPLETE_EMPTY;
          return base_reactor::update::COMPLETE_EMPTY;
        }
        m_previous_value = try_eval(*m_producer);
        m_sequence = sequence;
        m_update = base_reactor::update::NONE;
        return base_reactor::update::NONE;
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
      combine(m_update, base_reactor::update::COMPLETE);
    }
    m_sequence = sequence;
    return m_update;
  }

  template<typename T>
  typename fold_reactor<T>::type fold_reactor<T>::eval() const {
    return m_value.get();
  }
}

#endif
