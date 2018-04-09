#ifndef DARCEL_UTILITIES_HPP
#define DARCEL_UTILITIES_HPP
#include <memory>
#include <utility>

namespace darcel {
  template<typename T> class maybe;

  //! Performs a static cast on a unique ptr.
  template<typename T, typename U>
  std::unique_ptr<T> static_pointer_cast(std::unique_ptr<U>&& p) {
    return std::unique_ptr<T>(std::move(p));
  }

  //! Performs a dynamic cast on a unique ptr.
  template<typename T, typename U>
  std::unique_ptr<T> dynamic_pointer_cast(std::unique_ptr<U>&& p) {
    auto d = dynamic_cast<T*>(p.get());
    if(d != nullptr) {
      p.release();
      return std::unique_ptr<T>(d);
    }
    return {};
  }
}

#endif
