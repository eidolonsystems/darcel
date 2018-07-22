#ifndef DARCEL_UTILITIES_HPP
#define DARCEL_UTILITIES_HPP
#include <memory>
#include <utility>

namespace darcel {
  template<typename T> class Maybe;

  //! Performs a static cast on a unique ptr.
  template<typename T, typename U>
  std::unique_ptr<T> static_pointer_cast(std::unique_ptr<U> p) noexcept {
    return std::unique_ptr<T>(static_cast<T*>(p.release()));
  }

  //! Performs a dynamic cast on a unique ptr.
  template<typename T, typename U>
  std::unique_ptr<T> dynamic_pointer_cast(std::unique_ptr<U>&& p) noexcept {
    auto d = dynamic_cast<T*>(p.get());
    if(d != nullptr) {
      p.release();
      return std::unique_ptr<T>(d);
    }
    return {};
  }
}

#endif
