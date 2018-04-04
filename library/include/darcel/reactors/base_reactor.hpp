#ifndef DARCEL_BASE_REACTOR_HPP
#define DARCEL_BASE_REACTOR_HPP
#include <typeinfo>
#include "darcel/reactors/reactors.hpp"

namespace darcel {

  //! The untyped base class of a reactor.
  class base_reactor {
    public:

      //! Lists the types of updates a reactor can undergo after a commit.
      enum class update {

        //! No update.
        NONE = 1,

        //! The reactor has a new value.
        EVAL = 2,

        //! The reactor has come to an end.
        COMPLETE = 4,

        //! The reactor has terminated without an evaluation.
        COMPLETE_EMPTY = COMPLETE | NONE,

        //! The reactor has terminated with an evaluation.
        COMPLETE_EVAL = COMPLETE | EVAL
      };

      virtual ~base_reactor() = default;

      //! Returns the evaluation's type_info.
      virtual const std::type_info& get_type() const = 0;

      //! Commits changes to this reactor.
      /*!
        \param sequence The change's sequence number.
        \return A code representing the effect the commit had on this reactor.
      */
      virtual update commit(int sequence) = 0;

    protected:

      //! Constructs a base_reactor.
      base_reactor() = default;

    private:
      base_reactor(const base_reactor&) = delete;
      base_reactor& operator =(const base_reactor&) = delete;
  };

  //! Combines two reactor updates together.
  /*!
    \param lhs The update to modify.
    \param rhs The update to combine with lhs.
    \return <i>lhs</i>.
  */
  inline base_reactor::update combine(base_reactor::update& lhs,
      base_reactor::update rhs) {
    lhs = static_cast<base_reactor::update>(
      static_cast<int>(lhs) | static_cast<int>(rhs));
    return lhs;
  }

  //! Returns <code>true</code> iff a reactor update represents its completion.
  inline bool is_complete(base_reactor::update update) {
    return (static_cast<int>(update) &
      static_cast<int>(base_reactor::update::COMPLETE)) != 0;
  }

  //! Returns <code>true</code> iff a reactor update has an evaluation.
  inline bool has_eval(base_reactor::update update) {
    return (static_cast<int>(update) &
      static_cast<int>(base_reactor::update::EVAL)) != 0;
  }
}

#endif
