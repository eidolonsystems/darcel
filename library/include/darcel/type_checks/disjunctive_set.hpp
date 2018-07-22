#ifndef DARCEL_DISJUNCTIVE_SET_HPP
#define DARCEL_DISJUNCTIVE_SET_HPP
#include <vector>
#include "darcel/type_checks/conjunctive_set.hpp"
#include "darcel/type_checks/type_checks.hpp"
#include "darcel/type_checks/type_map.hpp"

namespace darcel {

  //! Stores a set of disjunctive requirements where only one of the
  //! requirements must be satisfied.
  class disjunctive_set {
    public:

      //! Constructs an empty set of requirements.
      disjunctive_set() = default;

      //! Tests if at least one requirement is satisfied using a specified
      //! variable type mapping.
      /*!
        \param t The type map used to test for satisfiability.
        \param s The scope used to find overloaded definitions.
        \return <code>true</code> if at least one requirement is satisfied using
                <i>t</i>.
      */
      bool is_satisfied(const type_map& t, const Scope& s) const;

      //! Adds a requirement that an expression must evaluate to a particular
      //! data type.
      /*!
        \param e The expression to constrain.
        \param t The data type that the expression must evaluate to.
      */
      void add(const Expression& e, std::shared_ptr<DataType> t);

      //! Adds a requirement that may be satisfied.
      /*!
        \param s The conjunctive set that may be satisfied.
      */
      void add(conjunctive_set s);

    private:
      std::vector<conjunctive_set> m_constraints;
  };

  inline bool disjunctive_set::is_satisfied(const type_map& t,
      const Scope& s) const {
    for(auto& constraint : m_constraints) {
      if(constraint.is_satisfied(t, s)) {
        return true;
      }
    }
    return false;
  }

  inline void disjunctive_set::add(const Expression& e,
      std::shared_ptr<DataType> t) {
    conjunctive_set c;
    c.add(e, std::move(t));
    add(std::move(c));
  }

  inline void disjunctive_set::add(conjunctive_set s) {
    m_constraints.push_back(std::move(s));
  }
}

#endif
