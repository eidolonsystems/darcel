#ifndef DARCEL_REACTOR_TRANSLATOR_HPP
#define DARCEL_REACTOR_TRANSLATOR_HPP
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include "darcel/data_types/function_data_type.hpp"
#include "darcel/data_types/generic_data_type.hpp"
#include "darcel/operations/clone_structure.hpp"
#include "darcel/operations/instantiate.hpp"
#include "darcel/reactors/chain_reactor.hpp"
#include "darcel/reactors/constant_reactor.hpp"
#include "darcel/reactors/operators.hpp"
#include "darcel/reactors/ostream_reactor.hpp"
#include "darcel/reactors/reactor_builder.hpp"
#include "darcel/reactors/reactors.hpp"
#include "darcel/reactors/trigger.hpp"
#include "darcel/syntax/syntax_nodes.hpp"
#include "darcel/syntax/syntax_node_visitor.hpp"
#include "darcel/type_checks/type_checker.hpp"

namespace darcel {

  //! Implements a syntax visitor to translate a node into a reactor.
  class reactor_translator final : private syntax_node_visitor {
    public:

      //! Function used to instantiate generic functions.
      /*!
        \param f The generic to instantiate.
      */
      using generic_builder = std::function<std::unique_ptr<reactor_builder>(
        const std::shared_ptr<function_definition>& f)>;

      //! Constructs a reactor translator.
      /*!
        \param s The global scope.
        \param t The trigger used to indicate reactor updates.
      */
      reactor_translator(const scope& s, trigger& t);

      //! Adds a definition to the translator.
      /*!
        \param v The variable to define.
        \param definition The definition of the variable.
      */
      void add(std::shared_ptr<variable> v,
        std::shared_ptr<reactor_builder> definition);

      //! Adds a definition to the translator.
      /*!
        \param f The function to define.
        \param definition The definition of the variable.
      */
      void add(std::shared_ptr<function_definition> v,
        std::shared_ptr<reactor_builder> definition);

      //! Adds a generic definition to the translator.
      /*!
        \param f The generic function being defined.
        \param definition The definition of the variable.
      */
      void add(std::shared_ptr<function_definition> f,
        generic_builder definition);

      //! Builds a reactor from a syntax node.
      /*!
        \param node The node to translate into a reactor.
      */
      void translate(const syntax_node& node);

      //! Returns the main reactor.
      std::shared_ptr<base_reactor> get_main() const;

      void visit(const bind_function_statement& node) override;

      void visit(const bind_variable_statement& node) override;

      void visit(const call_expression& node) override;

      void visit(const enum_expression& node) override;

      void visit(const function_expression& node) override;

      void visit(const literal_expression& node) override;

      void visit(const variable_expression& node) override;

    private:
      trigger* m_trigger;
      type_checker m_checker;
      std::shared_ptr<variable> m_main;
      std::unordered_map<std::shared_ptr<variable>,
        std::shared_ptr<reactor_builder>> m_variables;
      std::unordered_map<std::shared_ptr<function_definition>,
        std::shared_ptr<reactor_builder>> m_functions;
      std::unordered_map<std::shared_ptr<function_definition>,
        std::unique_ptr<bind_function_statement>> m_generic_definitions;
      std::unordered_map<std::shared_ptr<function_definition>, generic_builder>
        m_generic_builders;
      std::unordered_map<std::shared_ptr<variable>, std::shared_ptr<function>>
        m_overloads;
      std::shared_ptr<reactor_builder> m_evaluation;

      std::shared_ptr<reactor_builder> evaluate(const expression& e);
  };

  inline reactor_translator::reactor_translator(const scope& s, trigger& t)
      : m_trigger(&t),
        m_checker(s) {}

  inline void reactor_translator::add(std::shared_ptr<variable> v,
      std::shared_ptr<reactor_builder> definition) {
    m_variables.insert(std::make_pair(std::move(v), std::move(definition)));
  }

  inline void reactor_translator::add(std::shared_ptr<function_definition> f,
      std::shared_ptr<reactor_builder> definition) {
    m_functions.insert(std::make_pair(std::move(f), std::move(definition)));
  }

  inline void reactor_translator::add(std::shared_ptr<function_definition> f,
      generic_builder definition) {
  }

  inline void reactor_translator::translate(const syntax_node& node) {
    m_checker.check(node);
    node.apply(*this);
  }

  inline std::shared_ptr<base_reactor> reactor_translator::get_main() const {
    auto r = m_variables.find(m_main);
    if(r == m_variables.end()) {
      return nullptr;
    }
    return r->second->build(*m_trigger);
  }

  inline void reactor_translator::visit(const bind_function_statement& node) {
    struct parameter_reactor_builder final : reactor_builder {
      std::shared_ptr<reactor_builder> m_builder;

      void set_builder(std::shared_ptr<reactor_builder> builder) {
        m_builder = std::move(builder);
      }

      std::shared_ptr<base_reactor> build(
          const std::vector<std::shared_ptr<reactor_builder>>& parameters,
          trigger& t) const override {
        return m_builder->build(parameters, t);
      }
    };
    std::vector<std::shared_ptr<parameter_reactor_builder>> proxies;
    for(auto& parameter : node.get_parameters()) {
      proxies.push_back(std::make_shared<parameter_reactor_builder>());
      m_variables[parameter.m_variable] = proxies.back();
    }
    auto evaluation = evaluate(node.get_expression());
    auto builder = std::make_shared<function_reactor_builder>(
      [=] (auto& parameters, auto& t) {
        for(std::size_t i = 0; i < parameters.size(); ++i) {
          proxies[i]->set_builder(parameters[i]);
        }
        return evaluation->build(t);
      });
    auto definition = m_checker.get_definition(node);
    m_functions[definition] = std::move(builder);
    for(auto& parameter : node.get_parameters()) {
      m_variables.erase(parameter.m_variable);
    }
  }

  inline void reactor_translator::visit(const bind_variable_statement& node) {
    m_variables[node.get_variable()] = evaluate(node.get_expression());
    if(node.get_variable()->get_name() == "main") {
      m_main = node.get_variable();
    }
  }

  inline void reactor_translator::visit(const call_expression& node) {
    auto builder = evaluate(node.get_callable());
    std::vector<std::shared_ptr<reactor_builder>> parameters;
    for(auto& parameter : node.get_parameters()) {
      parameters.push_back(evaluate(*parameter));
    }
    m_evaluation = std::make_unique<function_reactor_builder>(
      [=, parameters = std::move(parameters)] (auto& p, auto& t) {
        return builder->build(parameters, t);
      });
  }

  inline void reactor_translator::visit(const enum_expression& node) {
    auto value = node.get_enum()->get_symbols()[node.get_index()].m_value;
    m_evaluation = make_constant_reactor_builder(value);
  }

  inline void reactor_translator::visit(const function_expression& node) {
    auto definition = m_checker.get_definition(node);
    if(is_generic(*definition->get_type())) {
      auto t = m_checker.get_type(node);

      // TODO
    }
    m_evaluation = m_functions[definition];
  }

  inline void reactor_translator::visit(const literal_expression& node) {
    struct literal_visitor : data_type_visitor {
      literal m_literal;
      std::unique_ptr<reactor_builder> m_builder;

      literal_visitor(literal l)
          : m_literal(std::move(l)) {}

      void visit(const bool_data_type& type) {
        if(m_literal.get_value() == "true") {
          m_builder = make_constant_reactor_builder(true);
        } else {
          m_builder = make_constant_reactor_builder(false);
        }
      }

      void visit(const float_data_type& type) {
        m_builder = make_constant_reactor_builder(
          std::stof(m_literal.get_value()));
      }

      void visit(const integer_data_type& type) {
        m_builder = make_constant_reactor_builder(
          std::stoi(m_literal.get_value()));
      }

      void visit(const text_data_type& type) {
        m_builder = make_constant_reactor_builder(m_literal.get_value());
      }
    };
    literal_visitor v(node.get_literal());
    node.get_literal().get_type()->apply(v);
    m_evaluation = std::move(v.m_builder);
  }

  inline void reactor_translator::visit(const variable_expression& node) {
    m_evaluation = m_variables[node.get_variable()];
  }

  inline std::shared_ptr<reactor_builder> reactor_translator::evaluate(
      const expression& e) {
    e.apply(*this);
    return m_evaluation;
  }
}

#endif
