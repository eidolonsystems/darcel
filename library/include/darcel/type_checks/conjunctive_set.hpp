#ifndef DARCEL_CONJUNCTIVE_SET_HPP
#define DARCEL_CONJUNCTIVE_SET_HPP
#include <memory>
#include <vector>
#include "darcel/data_types/data_type.hpp"
#include "darcel/syntax/expression.hpp"
#include "darcel/type_checks/type_checks.hpp"
#include "darcel/type_checks/type_map.hpp"

namespace darcel {

  //! Stores a set of requirements about an expression's data type where all
  //! requirements must be satisfied.
  class conjunctive_set {
    public:

      //! Constructs an empty set.
      conjunctive_set() = default;

      //! Tests if all requirements are satisfied using a specified mapping
      //! from variables to types.
      /*!
        \param t The type map used to test for satisfiability.
        \return true iff all requirements are satisfied using <i>t</i>.
      */
      bool is_satisfied(const type_map& t) const;

      //! Adds a requirement that an expression must evaluate to a particular
      //! data type.
      /*!
        \param e The expression to constrain.
        \param t The data type that the expression must evaluate to.
      */
      void add(const expression& e, std::shared_ptr<data_type> t);

    private:
      struct term {
        const expression* m_expression;
        std::shared_ptr<data_type> m_type;
      };
      std::vector<term> m_terms;
  };

  inline bool conjunctive_set::is_satisfied(const type_map& t) const {
    for(auto& term : m_terms) {
      try {
        auto term_type = t.get_type(*term.m_expression);
        if(term_type == nullptr || *term_type != *term.m_type) {
          return false;
        }
      } catch(const syntax_error&) {
        return false;
      }
    }
    return true;
  }

  inline void conjunctive_set::add(const expression& e,
      std::shared_ptr<data_type> t) {
    m_terms.push_back({&e, std::move(t)});
  }
}

#endif
