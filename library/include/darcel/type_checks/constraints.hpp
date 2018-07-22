#ifndef DARCEL_CONSTRAINTS_HPP
#define DARCEL_CONSTRAINTS_HPP
#include <vector>
#include "darcel/type_checks/disjunctive_set.hpp"
#include "darcel/type_checks/type_checks.hpp"

namespace darcel {

  //! Stores a set of data type/expression requirements where all requirements
  //! must be satisfied.
  class Constraints {
    public:

      //! Constructs an empty set of constraints.
      Constraints() = default;

      //! Tests if all requirements are satisfied using a specified mapping
      //! from variables to types.
      /*!
        \param t The type map used to test for satisfiability.
        \param s The scope used to find overloaded definitions.
        \return <code>true</code> if all requirements are satisfied using
                <i>t</i>.
      */
      bool is_satisfied(const TypeMap& t, const Scope& s) const;

      //! Adds a requirement that an expression must evaluate to a particular
      //! data type.
      /*!
        \param e The expression to constrain.
        \param t The data type that the expression must evaluate to.
      */
      void add(const Expression& e, std::shared_ptr<DataType> t);

      //! Adds a requirement that must be satisfied.
      /*!
        \param s The constraint that must be satisfied.
      */
      void add(ConjunctiveSet s);

      //! Adds a requirement that must be satisfied.
      /*!
        \param s The constraint that must be satisfied.
      */
      void add(DisjunctiveSet s);

    private:
      std::vector<DisjunctiveSet> m_constraints;
  };

  inline bool Constraints::is_satisfied(const TypeMap& t,
      const Scope& s) const {
    for(auto& constraint : m_constraints) {
      if(!constraint.is_satisfied(t, s)) {
        return false;
      }
    }
    return true;
  }

  inline void Constraints::add(const Expression& e,
      std::shared_ptr<DataType> t) {
    DisjunctiveSet d;
    d.add(e, std::move(t));
    add(std::move(d));
  }

  inline void Constraints::add(ConjunctiveSet s) {
    DisjunctiveSet d;
    d.add(std::move(s));
    add(d);
  }

  inline void Constraints::add(DisjunctiveSet s) {
    m_constraints.push_back(std::move(s));
  }
}

#endif
