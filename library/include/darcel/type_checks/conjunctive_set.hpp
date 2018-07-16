#ifndef DARCEL_CONJUNCTIVE_SET_HPP
#define DARCEL_CONJUNCTIVE_SET_HPP
#include <memory>
#include <vector>
#include "darcel/data_types/data_type.hpp"
#include "darcel/syntax/expression.hpp"
#include "darcel/type_checks/constraint_result.hpp"
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
        \param s The scope used to find overloaded definitions.
        \return A constraint result indicating whether all requirements are
                satisfied using <i>t</i>.
      */
      constraint_result is_satisfied(const type_map& t, const scope& s) const;

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

  inline constraint_result conjunctive_set::is_satisfied(const type_map& t,
      const scope& s) const {
    data_type_map<std::shared_ptr<generic_data_type>,
      std::shared_ptr<data_type>> substitutions;
    constraint_result result;
    for(auto& term : m_terms) {
      try {
        auto term_type = t.get_type(*term.m_expression);
        if(term_type == nullptr) {
          return result;
        }
        if(is_generic(*term_type)) {
          if(auto v = dynamic_cast<const variable_expression*>(
              term.m_expression)) {
            term_type = substitute(term_type, substitutions);
            result.m_conversions.insert(
              std::make_pair(v->get_variable(), term_type));
          } else {
            return result;
          }
        }
        auto expected_type = [&] {
          if(is_generic(*term.m_type)) {
            return substitute_generic(term.m_type, term_type, s, substitutions);
          } else {
            return term.m_type;
          }
        }();
        if(term_type == nullptr || *term_type != *expected_type) {
          return result;
        }
      } catch(const syntax_error&) {
        return result;
      }
    }
    result.m_is_satisfied = true;
    return result;
  }

  inline void conjunctive_set::add(const expression& e,
      std::shared_ptr<data_type> t) {
    m_terms.push_back({&e, std::move(t)});
  }
}

#endif
