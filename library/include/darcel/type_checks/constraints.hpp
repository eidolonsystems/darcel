#ifndef DARCEL_CONSTRAINTS_HPP
#define DARCEL_CONSTRAINTS_HPP
#include <vector>
#include "darcel/type_checks/disjunctive_set.hpp"
#include "darcel/type_checks/type_checks.hpp"

namespace darcel {

  //! Stores a set of data type/expression requirements where all requirements
  //! must be satisfied.
  class constraints {
    public:

      //! Constructs an empty set of constraints.
      constraints() = default;

      //! Tests if all requirements are satisfied using a specified mapping
      //! from variables to types.
      /*!
        \param t The type map used to test for satisfiability.
        \param s The scope used to find overloaded definitions.
        \return <code>true</code> if all requirements are satisfied using
                <i>t</i>.
      */
      bool is_satisfied(const type_map& t, const scope& s) const;

      //! Adds a requirement that an expression must evaluate to a particular
      //! data type.
      /*!
        \param e The expression to constrain.
        \param t The data type that the expression must evaluate to.
      */
      void add(const expression& e, std::shared_ptr<data_type> t);

      //! Adds a requirement that must be satisfied.
      /*!
        \param s The constraint that must be satisfied.
      */
      void add(conjunctive_set s);

      //! Adds a requirement that must be satisfied.
      /*!
        \param s The constraint that must be satisfied.
      */
      void add(disjunctive_set s);

    private:
      std::vector<disjunctive_set> m_constraints;
  };

  inline bool constraints::is_satisfied(const type_map& t,
      const scope& s) const {
    for(auto& constraint : m_constraints) {
      if(!constraint.is_satisfied(t, s)) {
        return false;
      }
    }
    return true;
  }

  inline void constraints::add(const expression& e,
      std::shared_ptr<data_type> t) {
    disjunctive_set d;
    d.add(e, std::move(t));
    add(std::move(d));
  }

  inline void constraints::add(conjunctive_set s) {
    disjunctive_set d;
    d.add(std::move(s));
    add(d);
  }

  inline void constraints::add(disjunctive_set s) {
    m_constraints.push_back(std::move(s));
  }
}

#endif
