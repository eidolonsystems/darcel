#ifndef DARCEL_MAYBE_HPP
#define DARCEL_MAYBE_HPP
#include <exception>
#include <type_traits>
#include <variant>
#include "darcel/utilities/utilities.hpp"

namespace darcel {

  /*! \brief Stores a value that could potentially result in an exception.
      \tparam T The type of value to store.
   */
  template<typename T>
  class Maybe {
    public:

      //! The type of value to store.
      using type = T;

      //! Constructs a Maybe.
      Maybe() = default;

      //! Constructs a Maybe with a normal value.
      /*!
        \param value The value to store.
      */
      Maybe(const type& value);

      //! Constructs a Maybe with a normal value.
      /*!
        \param value The value to store.
      */
      Maybe(type&& value);

      //! Constructs a Maybe with an exception.
      /*!
        \param exception The exception to throw.
      */
      Maybe(const std::exception_ptr& exception);

      //! Implicitly converts to the underlying value.
      operator const type& () const;

      //! Returns <code>true</code> iff a value is stored.
      bool has_value() const;

      //! Returns <code>true</code> iff an exception is stored.
      bool has_exception() const;

      //! Returns the stored value, or throws an exception.
      const type& get() const;

      //! Returns the stored value, or throws an exception.
      type& get();

      //! Returns the exception.
      std::exception_ptr get_exception() const;

      template<typename U>
      Maybe& operator =(const Maybe<U>& rhs);

      template<typename U>
      Maybe& operator =(Maybe<U>&& rhs);

      template<typename U>
      Maybe& operator =(const U& rhs);

      template<typename U>
      Maybe& operator =(U&& rhs);

    private:
      template<typename> friend class Maybe;
      std::variant<type, std::exception_ptr> m_value;
  };

  template<>
  class Maybe<void> {
    public:
      using type = void;

      Maybe() = default;

      Maybe(const std::exception_ptr& exception);

      bool has_exception() const;

      void get() const;

      std::exception_ptr get_exception() const;

    private:
      std::exception_ptr m_exception;
  };

  //! Tries calling a function, capturing any thrown exception.
  /*!
    \param f The function to call.
    \return The result of <i>f</i>.
  */
  template<typename F>
  Maybe<std::invoke_result_t<F>> try_call(F&& f) {
    try {
      if constexpr(std::is_same_v<std::invoke_result_t<F>, void>) {
        f();
        return {};
      } else {
        return f();
      }
    } catch(...) {
      return std::current_exception();
    }
  };

  template<typename T>
  Maybe<T>::Maybe(const type& value)
      : m_value(value) {}

  template<typename T>
  Maybe<T>::Maybe(type&& value)
      : m_value(std::move(value)) {}

  template<typename T>
  Maybe<T>::Maybe(const std::exception_ptr& exception)
      : m_value(exception) {}

  template<typename T>
  Maybe<T>::operator const typename Maybe<T>::type& () const {
    return get();
  }

  template<typename T>
  bool Maybe<T>::has_value() const {
    return m_value.index() == 0;
  }

  template<typename T>
  bool Maybe<T>::has_exception() const {
    return m_value.index() == 1;
  }

  template<typename T>
  const typename Maybe<T>::type& Maybe<T>::get() const {
    if(has_value()) {
      return std::get<type>(m_value);
    }
    std::rethrow_exception(std::get<std::exception_ptr>(m_value));
    throw std::exception();
  }

  template<typename T>
  typename Maybe<T>::type& Maybe<T>::get() {
    if(has_value()) {
      return std::get<type>(m_value);
    }
    std::rethrow_exception(std::get<std::exception_ptr>(m_value));
    throw std::exception();
  }

  template<typename T>
  std::exception_ptr Maybe<T>::get_exception() const {
    if(has_exception()) {
      return std::get<std::exception_ptr>(m_value);
    }
    return std::exception_ptr();
  }

  template<typename T>
  template<typename U>
  Maybe<T>& Maybe<T>::operator =(const Maybe<U>& rhs) {
    m_value = rhs.m_value;
    return *this;
  }

  template<typename T>
  template<typename U>
  Maybe<T>& Maybe<T>::operator =(Maybe<U>&& rhs) {
    m_value = std::move(rhs.m_value);
    return *this;
  }

  template<typename T>
  template<typename U>
  Maybe<T>& Maybe<T>::operator =(const U& rhs) {
    m_value = rhs;
    return *this;
  }

  template<typename T>
  template<typename U>
  Maybe<T>& Maybe<T>::operator =(U&& rhs) {
    m_value = std::move(rhs);
    return *this;
  }

  inline Maybe<void>::Maybe(const std::exception_ptr& exception)
      : m_exception(exception) {}

  inline bool Maybe<void>::has_exception() const {
    return m_exception != std::exception_ptr();
  }

  inline void Maybe<void>::get() const {
    if(m_exception != std::exception_ptr()) {
      std::rethrow_exception(m_exception);
    }
  }

  inline std::exception_ptr Maybe<void>::get_exception() const {
    return m_exception;
  }
}

#endif
