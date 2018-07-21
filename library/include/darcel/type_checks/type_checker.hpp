#ifndef DARCEL_TYPE_CHECKER_HPP
#define DARCEL_TYPE_CHECKER_HPP
#include <deque>
#include <unordered_map>
#include "darcel/data_types/callable_data_type.hpp"
#include "darcel/semantics/scope.hpp"
#include "darcel/semantics/variable.hpp"
#include "darcel/syntax/syntax_node_visitor.hpp"
#include "darcel/syntax/syntax_nodes.hpp"
#include "darcel/syntax/variable_not_found_error.hpp"
#include "darcel/type_checks/constraints.hpp"
#include "darcel/type_checks/function_overloads.hpp"
#include "darcel/type_checks/type_checks.hpp"
#include "darcel/type_checks/type_map.hpp"

namespace darcel {

  //! Performs type checking on syntax nodes.
  class type_checker {
    public:

      //! Constructs a type checker.
      type_checker();

      //! Constructs a type checker.
      /*!
        \param s The top level scope.
      */
      type_checker(const scope& s);

      //! Returns the type map used to track data types.
      const type_map& get_types() const;

      //! Returns a statement's definition.
      const std::shared_ptr<function_definition>& get_definition(
        const bind_function_statement& s) const;

      //! Returns an expression's particular overload.
      const std::shared_ptr<function_definition>& get_definition(
        const expression& e) const;

      //! Type checks a syntax node.
      void check(const syntax_node& node);

    private:
      std::deque<std::unique_ptr<scope>> m_scopes;
      type_map m_types;
      std::unordered_map<const bind_function_statement*,
        std::shared_ptr<function_definition>> m_definitions;
      std::unordered_map<const expression*,
        std::shared_ptr<function_definition>> m_call_definitions;

      scope& get_scope();
      scope& push_scope();
      void pop_scope();
  };

  //! Returns a list of all data types an expression can potentially evaluate
  //! to.
  /*!
    \param e The expression to analyze.
    \param t The map of types.
    \param s The scope used to get function definitions.
  */
  std::vector<std::shared_ptr<DataType>> determine_expression_types(
    const expression& e, const type_map& t, const scope& s);

  //! Returns a map of all possible generic substitutions in a function call.
  /*!
    \param generic The generic function being called.
    \param arguments The list of arguments being passed to the function.
  */  
  inline DataTypeMap<std::shared_ptr<GenericDataType>,
      std::vector<std::shared_ptr<DataType>>> resolve_generics(
      const std::shared_ptr<FunctionDataType>& generic,
      const std::vector<std::unique_ptr<expression>>& arguments,
      const type_map& types, const scope& s) {
    DataTypeMap<std::shared_ptr<GenericDataType>,
      std::vector<std::shared_ptr<DataType>>> substitutions;
    for(std::size_t i = 0; i != arguments.size(); ++i) {
      auto& parameter_type = generic->get_parameters()[i].m_type;
      if(!is_generic(*parameter_type)) {
        continue;
      }
      auto& argument = *arguments[i];
      auto argument_types = determine_expression_types(argument, types, s);
      for(auto& argument_type : argument_types) {
        DataTypeMap<std::shared_ptr<GenericDataType>,
          std::shared_ptr<DataType>> argument_substitutions;
        substitute_generic(parameter_type, argument_type, s,
          argument_substitutions);
        for(auto& argument_substitution : argument_substitutions) {
          substitutions[argument_substitution.first].push_back(
            argument_substitution.second);
        }
      }
    }
    return substitutions;
  }

  inline std::vector<std::shared_ptr<DataType>> determine_expression_types(
      const expression& e, const type_map& t, const scope& s) {
    struct expression_visitor final : syntax_node_visitor {
      const type_map* m_types;
      const scope* m_scope;
      std::vector<std::shared_ptr<DataType>> m_result;

      std::vector<std::shared_ptr<DataType>> operator ()(
          const expression& e, const type_map& t, const scope& s) {
        m_types = &t;
        m_scope = &s;
        e.apply(*this);
        return std::move(m_result);
      }

      void append(std::shared_ptr<DataType> type) {
        auto i = std::find_if(m_result.begin(), m_result.end(),
          equal_to(*type));
        if(i == m_result.end()) {
          m_result.push_back(std::move(type));
        }
      }

      void visit(const call_expression& node) override {
        auto overloads = determine_expression_types(node.get_callable(),
          *m_types, *m_scope);
        for(auto& overload : overloads) {
          if(auto f = std::dynamic_pointer_cast<FunctionDataType>(overload)) {
            if(f->get_parameters().size() != node.get_arguments().size()) {
              continue;
            }
            if(is_generic(*f->get_return_type())) {
              auto substitutions = resolve_generics(f, node.get_arguments(),
                *m_types, *m_scope);
              if(auto g = std::dynamic_pointer_cast<GenericDataType>(
                  f->get_return_type())) {

                // TODO : function generics
                auto& candidates = substitutions[g];
                for(auto& candidate : candidates) {
                  append(candidate);
                }
              }
            } else {
              append(f->get_return_type());
            }
          }
        }
      }

      void visit(const function_expression& node) override {
        m_scope->find(*node.get_function(),
          [&] (auto& definition) {
            append(definition.get_type());
            return false;
          });
        append(m_types->get_type(*node.get_function()));
      }

      void visit(const expression& node) override {
        auto t = m_types->get_type(node);
        if(t != nullptr) {
          append(std::move(t));
        }
      }
    };
    return expression_visitor()(e, t, s);
  }

  //! Performs type inference on an expression.
  /*!
    \param e The expression to infer.
    \param t The map of types.
    \param s The scope used to get function definitions.
  */
  inline type_map infer_types(const expression& e, const type_map& t,
      const scope& s) {
    struct constraints_visitor final : syntax_node_visitor {
      const type_map* m_types;
      const scope* m_scope;
      constraints m_constraints;
      std::unordered_map<std::shared_ptr<variable>,
        std::vector<std::shared_ptr<DataType>>> m_candidates;

      constraints_visitor(const expression& e, const type_map& t,
          const scope& s)
          : m_types(&t),
            m_scope(&s) {
        e.apply(*this);
      }

      void visit(const call_expression& node) override {
        node.get_callable().apply(*this);
        auto t = m_types->get_type(node.get_callable());
        std::vector<std::shared_ptr<FunctionDataType>> overloads;
        if(auto f = std::dynamic_pointer_cast<FunctionDataType>(t)) {
          overloads.push_back(std::move(f));
        } else {
          auto callable_type = std::static_pointer_cast<CallableDataType>(t);
          m_scope->find(*callable_type->get_function(),
            [&] (auto& definition) {
              overloads.push_back(definition.get_type());
              return false;
            });
        }
        disjunctive_set d;
        for(auto& overload : overloads) {
          if(overload->get_parameters().size() != node.get_arguments().size()) {
            continue;
          }
          conjunctive_set c;
          auto substitutions = resolve_generics(overload, node.get_arguments(),
            *m_types, *m_scope);
          for(std::size_t i = 0; i != node.get_arguments().size(); ++i) {
            auto& parameter_type = overload->get_parameters()[i].m_type;
            auto& argument = *node.get_arguments()[i];
            c.add(argument, parameter_type);
            if(auto v = dynamic_cast<const variable_expression*>(&argument)) {
              if(m_types->get_type(*v->get_variable()) == nullptr) {
                auto& candidates = m_candidates[v->get_variable()];
                auto generic_terms = extract_generic_terms(parameter_type);
                for(auto& generic_term : generic_terms) {
                  auto i = substitutions.find(generic_term);
                  if(i == substitutions.end()) {
                    continue;
                  }
                  candidates.insert(candidates.end(), i->second.begin(),
                    i->second.end());
                }
                if(candidates.empty()) {
                  candidates.push_back(parameter_type);
                }
              }
            }
          }
          d.add(std::move(c));
        }
        m_constraints.add(std::move(d));
        for(auto& argument : node.get_arguments()) {
          argument->apply(*this);
        }
      }
    };
    struct candidate_entry {
      std::shared_ptr<variable> m_variable;
      std::vector<std::shared_ptr<DataType>> m_candidates;
      std::size_t m_cycle_length;
    };
    constraints_visitor v(e, t, s);
    std::size_t total_combinations = 1;
    std::size_t index = 0;
    std::vector<candidate_entry> inferred_variables;
    for(auto& candidate : v.m_candidates) {
      auto cycle = [&] {
        if(inferred_variables.empty()) {
          return std::size_t(1);
        }
        return inferred_variables.back().m_cycle_length *
          inferred_variables.back().m_candidates.size();
      }();
      inferred_variables.push_back(
        {candidate.first, std::move(candidate.second), cycle});
      total_combinations *= cycle;
    }
    std::size_t i = 1;
    while(i <= total_combinations) {
      type_map candidate_map = t;
      for(std::size_t j = 0; j < inferred_variables.size(); ++j) {
        auto& entry = inferred_variables[j];
        auto index = (i / entry.m_cycle_length) % entry.m_candidates.size();
        candidate_map.add(*entry.m_variable, entry.m_candidates[index]);
      }
      if(v.m_constraints.is_satisfied(candidate_map, s)) {
        return candidate_map;
      }
      ++i;
    }
    return {};
  }

  inline type_checker::type_checker() {
    m_scopes.push_back(std::make_unique<scope>());
  }

  inline type_checker::type_checker(const scope& s) {
    m_scopes.push_back(std::make_unique<scope>(&s));
    get_scope().find(
      [&] (auto& e) {
        if(auto definition =
            std::dynamic_pointer_cast<function_definition>(e)) {
          m_types.add(std::move(definition));
        } else if(auto f = std::dynamic_pointer_cast<function>(e)) {
          auto callable = std::static_pointer_cast<CallableDataType>(
            get_scope().find(
            [&] (auto& t) {
              auto callable = std::dynamic_pointer_cast<CallableDataType>(t);
              return callable != nullptr && callable->get_function() == f;
            }));
          if(callable == nullptr) {
            callable = std::make_shared<CallableDataType>(f);
          }
          m_types.add(*f, std::move(callable));
        }
        return false;
      });
  }

  inline const type_map& type_checker::get_types() const {
    return m_types;
  }

  inline const std::shared_ptr<function_definition>&
      type_checker::get_definition(const bind_function_statement& s) const {
    auto i = m_definitions.find(&s);
    if(i == m_definitions.end()) {
      static const std::shared_ptr<function_definition> NONE;
      return NONE;
    }
    return i->second;
  }

  inline const std::shared_ptr<function_definition>&
      type_checker::get_definition(const expression& e) const {
    auto i = m_call_definitions.find(&e);
    if(i == m_call_definitions.end()) {
      static const std::shared_ptr<function_definition> NONE;
      return NONE;
    }
    return i->second;
  }

  inline void type_checker::check(const syntax_node& node) {
    struct type_check_visitor final : syntax_node_visitor {
      type_checker* m_checker;
      std::shared_ptr<DataType> m_last;

      void operator ()(type_checker& checker, const syntax_node& node) {
        m_checker = &checker;
        node.apply(*this);
      }

      void visit(const bind_function_statement& node) override {
        std::vector<FunctionDataType::Parameter> parameters;
        auto infer = false;
        for(auto& parameter : node.get_parameters()) {
          parameters.emplace_back(parameter.m_variable->get_name(),
            parameter.m_type);
          if(parameter.m_type != nullptr) {
            m_checker->m_types.add(*parameter.m_variable, parameter.m_type);
          } else {
            infer = true;
          }
        }
        m_checker->get_scope().add(node.get_function());
        if(infer) {
          auto inference = infer_types(node.get_expression(),
            m_checker->get_types(), m_checker->get_scope());
          for(std::size_t i = 0; i != parameters.size(); ++i) {
            if(parameters[i].m_type == nullptr) {
              auto inferred_type = inference.get_type(
                *node.get_parameters()[i].m_variable);
              if(inferred_type == nullptr) {

                // TODO: Compiler error
                throw syntax_error(
                  syntax_error_code::FUNCTION_PARAMETER_ALREADY_DEFINED,
                  node.get_parameters()[i].m_variable->get_location());
              }
              parameters[i].m_type = inferred_type;
            }
          }
          m_checker->m_types = std::move(inference);
        }
        node.get_expression().apply(*this);
        if(m_checker->m_types.get_type(*node.get_function()) == nullptr) {
          auto callable_type = std::make_shared<CallableDataType>(
            node.get_function());
          m_checker->m_types.add(*node.get_function(), callable_type);
          m_checker->get_scope().add(callable_type);
        }
        auto t = std::make_shared<FunctionDataType>(std::move(parameters),
          std::move(m_last));
        auto definition = std::make_shared<function_definition>(
          node.get_location(), node.get_function(), std::move(t));
        m_checker->get_scope().add(definition);
        m_checker->m_types.add(definition);
        m_checker->m_definitions.insert(std::make_pair(&node, definition));
      }

      void visit(const bind_variable_statement& node) override {
        node.get_expression().apply(*this);
        m_checker->m_types.add(*node.get_variable(), std::move(m_last));
        m_checker->get_scope().add(node.get_variable());
      }

      void visit(const call_expression& node) override {
        node.get_callable().apply(*this);
        auto t = std::move(m_last);
        if(auto callable_type =
            std::dynamic_pointer_cast<CallableDataType>(t)) {
          std::vector<FunctionDataType::Parameter> parameters;
          for(auto& argument : node.get_arguments()) {
            argument->apply(*this);
            parameters.emplace_back("", std::move(m_last));
          }
          auto overload = find_overload(*callable_type->get_function(),
            parameters, m_checker->get_scope());
          if(overload == nullptr) {
            throw syntax_error(syntax_error_code::OVERLOAD_NOT_FOUND,
              node.get_callable().get_location());
          }
          auto instance = instantiate(*overload, parameters,
            m_checker->get_scope());
          for(std::size_t i = 0; i < parameters.size(); ++i) {
            auto& p = parameters[i].m_type;
            auto& o = instance->get_parameters()[i].m_type;
            if(auto callable_type =
                std::dynamic_pointer_cast<CallableDataType>(p)) {
              if(auto signature =
                  std::dynamic_pointer_cast<FunctionDataType>(o)) {
                auto call_overload = find_overload(
                  *callable_type->get_function(), *signature,
                  m_checker->get_scope());
                m_checker->m_types.add(*node.get_arguments()[i],
                  std::move(signature));
                m_checker->m_call_definitions.insert(
                  std::make_pair(node.get_arguments()[i].get(),
                  std::move(call_overload)));
              }
            }
          }
          m_checker->m_types.add(node.get_callable(), std::move(instance));
          m_checker->m_call_definitions.insert(
            std::make_pair(&node.get_callable(), std::move(overload)));
        }
        visit(static_cast<const expression&>(node));
      }

      void visit(const expression& node) override {
        m_last = m_checker->m_types.get_type(node);
      }

      void visit(const function_expression& node) override {
        m_last = std::make_shared<CallableDataType>(node.get_function());
      }
    };
    type_check_visitor()(*this, node);
  }

  inline scope& type_checker::get_scope() {
    return *m_scopes.back();
  }

  inline scope& type_checker::push_scope() {
    m_scopes.push_back(std::make_unique<scope>(&get_scope()));
  }

  inline void type_checker::pop_scope() {
    m_scopes.pop_back();
  }
}

#endif
