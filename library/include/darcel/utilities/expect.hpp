#ifndef DARCEL_EXPECT_HPP
#define DARCEL_EXPECT_HPP
#include <exception>
#include <variant>
#include "darcel/utilities/utilities.hpp"

namespace darcel {

  /*! \brief Stores a value that could potentially result in an exception.
      \tparam T The type of value to store.
   */
  template<typename T>
  class expect {
    public:

      //! The type of value to store.
      using type = T;

      //! Constructs an expect.
      expect() = default;

      //! Constructs an expect with a normal value.
      /*!
        \param value The value to store.
      */
      expect(const type& value);

      //! Constructs an expect with a normal value.
      /*!
        \param value The value to store.
      */
      expect(type&& value);

      //! Constructs an expect with an exception.
      /*!
        \param exception The exception to throw.
      */
      expect(const std::exception_ptr& exception);

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
      expect& operator =(const expect<U>& rhs);

      template<typename U>
      expect& operator =(expect<U>&& rhs);

      template<typename U>
      expect& operator =(const U& rhs);

      template<typename U>
      expect& operator =(U&& rhs);

    private:
      template<typename> friend class expect;
      std::variant<type, std::exception_ptr> m_value;
  };

  template<>
  class expect<void> {
    public:
      using type = void;

      expect() = default;

      expect(const std::exception_ptr& exception);

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
  expect<std::invoke_result_t<F>> try_call(F&& f) {
    try {
      return f();
    } catch(...) {
      return std::current_exception();
    }
  };

  template<typename T>
  expect<T>::expect(const type& value)
      : m_value(value) {}

  template<typename T>
  expect<T>::expect(type&& value)
      : m_value(std::move(value)) {}

  template<typename T>
  expect<T>::expect(const std::exception_ptr& exception)
      : m_value(exception) {}

  template<typename T>
  expect<T>::operator const typename expect<T>::type& () const {
    return get();
  }

  template<typename T>
  bool expect<T>::has_value() const {
    return m_value.index() == 0;
  }

  template<typename T>
  bool expect<T>::has_exception() const {
    return m_value.index() == 1;
  }

  template<typename T>
  const typename expect<T>::type& expect<T>::get() const {
    if(has_value()) {
      return std::get<type>(m_value);
    }
    std::rethrow_exception(std::get<std::exception_ptr>(m_value));
    throw std::exception();
  }

  template<typename T>
  typename expect<T>::type& expect<T>::get() {
    if(has_value()) {
      return std::get<type>(m_value);
    }
    std::rethrow_exception(std::get<std::exception_ptr>(m_value));
    throw std::exception();
  }

  template<typename T>
  std::exception_ptr expect<T>::get_exception() const {
    if(has_exception()) {
      return std::get<std::exception_ptr>(m_value);
    }
    return std::exception_ptr();
  }

  template<typename T>
  template<typename U>
  expect<T>& expect<T>::operator =(const expect<U>& rhs) {
    m_value = rhs.m_value;
    return *this;
  }

  template<typename T>
  template<typename U>
  expect<T>& expect<T>::operator =(expect<U>&& rhs) {
    m_value = std::move(rhs.m_value);
    return *this;
  }

  template<typename T>
  template<typename U>
  expect<T>& expect<T>::operator =(const U& rhs) {
    m_value = rhs;
    return *this;
  }

  template<typename T>
  template<typename U>
  expect<T>& expect<T>::operator =(U&& rhs) {
    m_value = std::move(rhs);
    return *this;
  }

  inline expect<void>::expect(const std::exception_ptr& exception)
      : m_exception(exception) {}

  inline bool expect<void>::has_exception() const {
    return m_exception != std::exception_ptr();
  }

  inline void expect<void>::get() const {
    if(m_exception != std::exception_ptr()) {
      std::rethrow_exception(m_exception);
    }
  }

  inline std::exception_ptr expect<void>::get_exception() const {
    return m_exception;
  }
}

#endif
