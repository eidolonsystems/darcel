#ifndef DARCEL_MAYBE_HPP
#define DARCEL_MAYBE_HPP
#include <exception>
#include <variant>
#include "darcel/utilities/utilities.hpp"

namespace darcel {

  /*! \brief Stores a value that could potentially result in an exception.
      \tparam T The type of value to store.
   */
  template<typename T>
  class maybe {
    public:

      //! The type of value to store.
      using type = T;

      //! Constructs a maybe.
      maybe() = default;

      //! Constructs a maybe with a normal value.
      /*!
        \param value The value to store.
      */
      maybe(const type& value);

      //! Constructs a maybe with a normal value.
      /*!
        \param value The value to store.
      */
      maybe(type&& value);

      //! Constructs a maybe with an exception.
      /*!
        \param exception The exception to throw.
      */
      maybe(const std::exception_ptr& exception);

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
      maybe& operator =(const maybe<U>& rhs);

      template<typename U>
      maybe& operator =(maybe<U>&& rhs);

      template<typename U>
      maybe& operator =(const U& rhs);

      template<typename U>
      maybe& operator =(U&& rhs);

    private:
      template<typename> friend class maybe;
      std::variant<type, std::exception_ptr> m_value;
  };

  template<>
  class maybe<void> {
    public:
      using type = void;

      maybe() = default;

      maybe(const std::exception_ptr& exception);

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
  maybe<std::invoke_result_t<F>> try_call(F&& f) {
    try {
      return f();
    } catch(...) {
      return std::current_exception();
    }
  };

  template<typename T>
  maybe<T>::maybe(const type& value)
      : m_value(value) {}

  template<typename T>
  maybe<T>::maybe(type&& value)
      : m_value(std::move(value)) {}

  template<typename T>
  maybe<T>::maybe(const std::exception_ptr& exception)
      : m_value(exception) {}

  template<typename T>
  maybe<T>::operator const typename maybe<T>::type& () const {
    return get();
  }

  template<typename T>
  bool maybe<T>::has_value() const {
    return m_value.index() == 0;
  }

  template<typename T>
  bool maybe<T>::has_exception() const {
    return m_value.index() == 1;
  }

  template<typename T>
  const typename maybe<T>::type& maybe<T>::get() const {
    if(has_value()) {
      return std::get<type>(m_value);
    }
    std::rethrow_exception(std::get<std::exception_ptr>(m_value));
    throw std::exception();
  }

  template<typename T>
  typename maybe<T>::type& maybe<T>::get() {
    if(has_value()) {
      return std::get<type>(m_value);
    }
    std::rethrow_exception(std::get<std::exception_ptr>(m_value));
    throw std::exception();
  }

  template<typename T>
  std::exception_ptr maybe<T>::get_exception() const {
    if(has_exception()) {
      return std::get<std::exception_ptr>(m_value);
    }
    return std::exception_ptr();
  }

  template<typename T>
  template<typename U>
  maybe<T>& maybe<T>::operator =(const maybe<U>& rhs) {
    m_value = rhs.m_value;
    return *this;
  }

  template<typename T>
  template<typename U>
  maybe<T>& maybe<T>::operator =(maybe<U>&& rhs) {
    m_value = std::move(rhs.m_value);
    return *this;
  }

  template<typename T>
  template<typename U>
  maybe<T>& maybe<T>::operator =(const U& rhs) {
    m_value = rhs;
    return *this;
  }

  template<typename T>
  template<typename U>
  maybe<T>& maybe<T>::operator =(U&& rhs) {
    m_value = std::move(rhs);
    return *this;
  }

  inline maybe<void>::maybe(const std::exception_ptr& exception)
      : m_exception(exception) {}

  inline bool maybe<void>::has_exception() const {
    return m_exception != std::exception_ptr();
  }

  inline void maybe<void>::get() const {
    if(m_exception != std::exception_ptr()) {
      std::rethrow_exception(m_exception);
    }
  }

  inline std::exception_ptr maybe<void>::get_exception() const {
    return m_exception;
  }
}

#endif
