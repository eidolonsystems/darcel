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
  class ReactorTranslator final : private syntax_node_visitor {
    public:

      //! Function used to instantiate generic functions.
      /*!
        \param f The data type to instantiate.
      */
      using generic_builder = std::function<std::unique_ptr<ReactorBuilder>(
        const std::shared_ptr<FunctionDataType>& f)>;

      //! Constructs a reactor translator.
      /*!
        \param s The global scope.
      */
      ReactorTranslator(const Scope& s);

      //! Adds a definition to the translator.
      /*!
        \param v The variable to define.
        \param definition The definition of the variable.
      */
      void add(std::shared_ptr<Variable> v,
        std::shared_ptr<ReactorBuilder> definition);

      //! Adds a definition to the translator.
      /*!
        \param f The function to define.
        \param definition The definition of the variable.
      */
      void add(std::shared_ptr<FunctionDefinition> v,
        std::shared_ptr<ReactorBuilder> definition);

      //! Adds a generic definition to the translator.
      /*!
        \param f The generic function being defined.
        \param definition The definition of the variable.
      */
      void add(std::shared_ptr<FunctionDefinition> f,
        generic_builder definition);

      //! Builds a reactor from a syntax node.
      /*!
        \param node The node to translate into a reactor.
      */
      void translate(const syntax_node& node);

      //! Returns the main reactor.
      /*!
        \param t The trigger used to execute the main reactor.
      */
      std::shared_ptr<BaseReactor> get_main(Trigger& t) const;

      void visit(const bind_function_statement& node) override;

      void visit(const bind_variable_statement& node) override;

      void visit(const call_expression& node) override;

      void visit(const enum_expression& node) override;

      void visit(const function_expression& node) override;

      void visit(const literal_expression& node) override;

      void visit(const variable_expression& node) override;

    private:
      struct generic_definition_builder {
        std::shared_ptr<bind_function_statement> m_node;
        std::shared_ptr<FunctionDefinition> m_definition;
        ReactorTranslator* m_translator;
        std::shared_ptr<DataTypeMap<std::shared_ptr<FunctionDataType>,
          std::unique_ptr<bind_function_statement>>> m_instantiations;

        generic_definition_builder(const bind_function_statement& node,
          std::shared_ptr<FunctionDefinition> definition,
          ReactorTranslator& translator);

        std::unique_ptr<ReactorBuilder> operator ()(
          const std::shared_ptr<FunctionDataType>& t) const;
      };
      type_checker m_checker;
      std::shared_ptr<Variable> m_main;
      std::unordered_map<std::shared_ptr<Variable>,
        std::shared_ptr<ReactorBuilder>> m_variables;
      std::unordered_map<std::shared_ptr<FunctionDefinition>,
        std::shared_ptr<ReactorBuilder>> m_functions;
      std::unordered_map<std::shared_ptr<FunctionDefinition>, generic_builder>
        m_generic_builders;
      std::shared_ptr<ReactorBuilder> m_evaluation;

      std::shared_ptr<ReactorBuilder> evaluate(const expression& e);
  };

  inline ReactorTranslator::generic_definition_builder::
      generic_definition_builder(const bind_function_statement& node,
      std::shared_ptr<FunctionDefinition> definition,
      ReactorTranslator& translator)
      : m_node(clone_structure(node)),
        m_definition(std::move(definition)),
        m_translator(&translator),
        m_instantiations(std::make_shared<
          DataTypeMap<std::shared_ptr<FunctionDataType>,
          std::unique_ptr<bind_function_statement>>>()) {}

  inline std::unique_ptr<ReactorBuilder>
      ReactorTranslator::generic_definition_builder::operator ()(
      const std::shared_ptr<FunctionDataType>& t) const {
    auto i = m_instantiations->find(t);
    if(i == m_instantiations->end()) {
      auto instantiation = darcel::instantiate(*m_node, *t);
      m_translator->m_checker.check(*instantiation);
      instantiation->apply(*m_translator);
      i = m_instantiations->insert(
        std::make_pair(t, std::move(instantiation))).first;
    }
    auto& instantiation = i->second;
    auto definition = m_translator->m_checker.get_definition(*instantiation);
    auto builder = m_translator->m_functions[definition];
    return std::make_unique<FunctionReactorBuilder>(
      [=] (auto& parameters, auto& t) {
        return builder->build(parameters, t);
      });
  }

  inline ReactorTranslator::ReactorTranslator(const Scope& s)
      : m_checker(s) {}

  inline void ReactorTranslator::add(std::shared_ptr<Variable> v,
      std::shared_ptr<ReactorBuilder> definition) {
    m_variables.insert(std::make_pair(std::move(v), std::move(definition)));
  }

  inline void ReactorTranslator::add(std::shared_ptr<FunctionDefinition> f,
      std::shared_ptr<ReactorBuilder> definition) {
    m_functions.insert(std::make_pair(std::move(f), std::move(definition)));
  }

  inline void ReactorTranslator::add(std::shared_ptr<FunctionDefinition> f,
      generic_builder definition) {
    m_generic_builders.insert(
      std::make_pair(std::move(f), std::move(definition)));
  }

  inline void ReactorTranslator::translate(const syntax_node& node) {
    m_checker.check(node);
    node.apply(*this);
  }

  inline std::shared_ptr<BaseReactor> ReactorTranslator::get_main(
      Trigger& t) const {
    auto r = m_variables.find(m_main);
    if(r == m_variables.end()) {
      return nullptr;
    }
    return r->second->build(t);
  }

  inline void ReactorTranslator::visit(const bind_function_statement& node) {
    struct parameter_reactor_builder final : ReactorBuilder {
      std::shared_ptr<ReactorBuilder> m_builder;

      void set_builder(std::shared_ptr<ReactorBuilder> builder) {
        m_builder = std::move(builder);
      }

      std::shared_ptr<BaseReactor> build(
          const std::vector<std::shared_ptr<ReactorBuilder>>& parameters,
          Trigger& t) const override {
        return m_builder->build(parameters, t);
      }
    };
    auto definition = m_checker.get_definition(node);
    if(is_generic(*definition->get_type())) {
      auto builder = generic_definition_builder(node, definition, *this);
      m_generic_builders.insert(std::make_pair(definition, std::move(builder)));
      return;
    }
    std::vector<std::shared_ptr<parameter_reactor_builder>> proxies;
    for(auto& parameter : node.get_parameters()) {
      proxies.push_back(std::make_shared<parameter_reactor_builder>());
      m_variables[parameter.m_variable] = proxies.back();
    }
    auto evaluation = evaluate(node.get_expression());
    auto builder = std::make_shared<FunctionReactorBuilder>(
      [=] (auto& parameters, auto& t) {
        for(std::size_t i = 0; i < parameters.size(); ++i) {
          proxies[i]->set_builder(parameters[i]);
        }
        return evaluation->build(t);
      });
    m_functions[definition] = std::move(builder);
    for(auto& parameter : node.get_parameters()) {
      m_variables.erase(parameter.m_variable);
    }
  }

  inline void ReactorTranslator::visit(const bind_variable_statement& node) {
    auto evaluation = evaluate(node.get_expression());
    if(evaluation != nullptr) {
      m_variables[node.get_variable()] = std::move(evaluation);
    }
    if(node.get_variable()->get_name() == "main") {
      m_main = node.get_variable();
    }
  }

  inline void ReactorTranslator::visit(const call_expression& node) {
    auto builder = evaluate(node.get_callable());
    std::vector<std::shared_ptr<ReactorBuilder>> arguments;
    for(auto& argument : node.get_arguments()) {
      arguments.push_back(evaluate(*argument));
    }
    m_evaluation = std::make_unique<FunctionReactorBuilder>(
      [=, arguments = std::move(arguments)] (auto& p, auto& t) {
        return builder->build(arguments, t);
      });
  }

  inline void ReactorTranslator::visit(const enum_expression& node) {
    auto value = node.get_enum()->get_symbols()[node.get_index()].m_value;
    m_evaluation = make_constant_reactor_builder(value);
  }

  inline void ReactorTranslator::visit(const function_expression& node) {
    auto definition = m_checker.get_definition(node);
    if(definition == nullptr) {
      return;
    }
    if(is_generic(*definition->get_type())) {
      auto t = std::static_pointer_cast<FunctionDataType>(
        m_checker.get_types().get_type(node));
      auto& builder = m_generic_builders.at(definition);
      m_evaluation = builder(t);
    } else {
      m_evaluation = m_functions[definition];
    }
  }

  inline void ReactorTranslator::visit(const literal_expression& node) {
    struct literal_visitor : DataTypeVisitor {
      Literal m_literal;
      std::unique_ptr<ReactorBuilder> m_builder;

      literal_visitor(Literal l)
          : m_literal(std::move(l)) {}

      void visit(const BoolDataType& type) {
        if(m_literal.get_value() == "true") {
          m_builder = make_constant_reactor_builder(true);
        } else {
          m_builder = make_constant_reactor_builder(false);
        }
      }

      void visit(const FloatDataType& type) {
        m_builder = make_constant_reactor_builder(
          std::stof(m_literal.get_value()));
      }

      void visit(const IntegerDataType& type) {
        m_builder = make_constant_reactor_builder(
          std::stoi(m_literal.get_value()));
      }

      void visit(const TextDataType& type) {
        m_builder = make_constant_reactor_builder(m_literal.get_value());
      }
    };
    literal_visitor v(node.get_literal());
    node.get_literal().get_type()->apply(v);
    m_evaluation = std::move(v.m_builder);
  }

  inline void ReactorTranslator::visit(const variable_expression& node) {
    if(auto callable_type = std::dynamic_pointer_cast<CallableDataType>(
        m_checker.get_types().get_type(*node.get_variable()))) {
      auto definition = m_checker.get_definition(node);
      m_evaluation = m_functions[definition];
    } else {
      m_evaluation = m_variables.at(node.get_variable());
    }
  }

  inline std::shared_ptr<ReactorBuilder> ReactorTranslator::evaluate(
      const expression& e) {
    e.apply(*this);
    auto evaluation = std::move(m_evaluation);
    return evaluation;
  }
}

#endif
