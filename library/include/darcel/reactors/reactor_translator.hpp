#ifndef DARCEL_REACTOR_TRANSLATOR_HPP
#define DARCEL_REACTOR_TRANSLATOR_HPP
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include "darcel/data_types/function_data_type.hpp"
#include "darcel/reactors/chain_reactor.hpp"
#include "darcel/reactors/constant_reactor.hpp"
#include "darcel/reactors/operators.hpp"
#include "darcel/reactors/ostream_reactor.hpp"
#include "darcel/reactors/reactor_builder.hpp"
#include "darcel/reactors/reactors.hpp"
#include "darcel/reactors/trigger.hpp"
#include "darcel/semantics/scope.hpp"
#include "darcel/syntax/syntax_nodes.hpp"
#include "darcel/syntax/syntax_node_visitor.hpp"

namespace darcel {

  //! Implements a syntax visitor to translate a node into a reactor.
  class reactor_translator : private syntax_node_visitor {
    public:

      //! Constructs a reactor translator given a scope containing all of the
      //! built-in global definitions.
      /*!
        \param s The scope the node belongs to.
        \param t The trigger used to indicate reactor updates.
      */
      reactor_translator(const scope& s, trigger& t);

      //! Builds a reactor from a syntax node.
      /*!
        \param node The node to translate into a reactor.
      */
      void translate(const syntax_node& node);

      //! Returns the main reactor.
      std::shared_ptr<base_reactor> get_main() const;

      void visit(const bind_function_statement& node) override final;

      void visit(const bind_variable_statement& node) override final;

      void visit(const call_expression& node) override final;

      void visit(const literal_expression& node) override final;

      void visit(const variable_expression& node) override final;

    private:
      const scope* m_scope;
      trigger* m_trigger;
      std::shared_ptr<variable> m_main;
      std::unordered_map<std::shared_ptr<variable>,
        std::shared_ptr<reactor_builder>> m_variables;
      std::shared_ptr<reactor_builder> m_evaluation;

      std::shared_ptr<reactor_builder> evaluate(const expression& e);
      std::shared_ptr<reactor_builder> instantiate(std::shared_ptr<variable> v);
  };

  inline reactor_translator::reactor_translator(const scope& s, trigger& t)
      : m_scope(&s),
        m_trigger(&t) {
    auto add = m_scope->find<function>("add");
    if(add != nullptr) {
      for(auto& overload : add->get_overloads()) {
        auto signature = std::static_pointer_cast<function_data_type>(
          overload->get_data_type());
        if(signature->get_parameters().size() == 2) {
          if(*signature->get_parameters()[0].m_type == integer_data_type() &&
              *signature->get_parameters()[1].m_type == integer_data_type()) {
            m_variables[overload] = make_add_reactor_builder<int, int>();
          } else if(
              *signature->get_parameters()[0].m_type == float_data_type() &&
              *signature->get_parameters()[1].m_type == float_data_type()) {
            m_variables[overload] = make_add_reactor_builder<double, double>();
          } else if(
              *signature->get_parameters()[0].m_type == text_data_type() &&
              *signature->get_parameters()[1].m_type == text_data_type()) {
            m_variables[overload] =
              make_add_reactor_builder<std::string, std::string>();
          }
        }
      }
    }
  }

  inline void reactor_translator::translate(const syntax_node& node) {
    node.apply(*this);
  }

  inline std::shared_ptr<base_reactor> reactor_translator::get_main() const {
    auto r = m_variables.find(m_main);
    if(r == m_variables.end()) {
      return nullptr;
    }
    return r->second->build({}, *m_trigger);
  }

  inline void reactor_translator::visit(const bind_function_statement& node) {
    class parameter_reactor_builder : public reactor_builder {
      public:
        void set_reactor(std::shared_ptr<base_reactor> r) {
          m_reactor = std::move(r);
        }

        std::shared_ptr<base_reactor> build(
            const std::vector<std::shared_ptr<base_reactor>>& parameters,
            trigger& t) const override final {
          return m_reactor;
        }

      private:
        std::shared_ptr<base_reactor> m_reactor;
    };
    std::vector<std::shared_ptr<parameter_reactor_builder>> proxies;
    for(auto& parameter : node.get_parameters()) {
      proxies.push_back(std::make_shared<parameter_reactor_builder>());
      m_variables[parameter] = proxies.back();
    }
    auto evaluation = evaluate(node.get_expression());
    auto builder = std::make_shared<function_reactor_builder>(
      [=] (auto& parameters, auto& t) {
        for(std::size_t i = 0; i < parameters.size(); ++i) {
          proxies[i]->set_reactor(parameters[i]);
        }
        return evaluation->build(t);
      });
    m_variables[node.get_overload()] = std::move(builder);
    for(auto& parameter : node.get_parameters()) {
      m_variables.erase(parameter);
    }
  }

  inline void reactor_translator::visit(const bind_variable_statement& node) {
    auto reactor = evaluate(node.get_expression())->build(*m_trigger);
    auto builder = std::make_shared<function_reactor_builder>(
      [=] (auto& parameters, auto& t) {
        return reactor;
      });
    m_variables[node.get_variable()] = std::move(builder);
    if(node.get_variable()->get_name() == "main") {
      m_main = node.get_variable();
    }
  }

  inline void reactor_translator::visit(const call_expression& node) {
    auto builder = evaluate(node.get_callable());
    std::vector<std::shared_ptr<reactor_builder>> parameter_builders;
    for(auto& parameter : node.get_parameters()) {
      parameter_builders.push_back(evaluate(*parameter));
    }
    m_evaluation = std::make_unique<function_reactor_builder>(
      [=] (auto& parameters, auto& t) {
        std::vector<std::shared_ptr<base_reactor>> arguments;
        for(auto& parameter_builder : parameter_builders) {
          arguments.push_back(parameter_builder->build(t));
        }
        return builder->build(arguments, t);
      });
  }

  inline void reactor_translator::visit(const literal_expression& node) {
    struct literal_visitor : data_type_visitor {
      literal m_literal;
      std::unique_ptr<reactor_builder> m_reactor;

      literal_visitor(literal l)
          : m_literal(std::move(l)) {}

      void visit(const bool_data_type& type) {
        if(m_literal.get_value() == "true") {
          m_reactor = make_constant_reactor_builder(true);
        } else {
          m_reactor = make_constant_reactor_builder(false);
        }
      }

      void visit(const float_data_type& type) {
        m_reactor = make_constant_reactor_builder(
          std::stof(m_literal.get_value()));
      }

      void visit(const integer_data_type& type) {
        m_reactor = make_constant_reactor_builder(
          std::stoi(m_literal.get_value()));
      }

      void visit(const text_data_type& type) {
        m_reactor = make_constant_reactor_builder(m_literal.get_value());
      }
    };
    literal_visitor v(node.get_literal());
    node.get_data_type()->apply(v);
    m_evaluation = std::move(v.m_reactor);
  }

  inline void reactor_translator::visit(const variable_expression& node) {
    auto evaluation = m_variables.find(node.get_variable());
    if(evaluation != m_variables.end()) {
      m_evaluation = evaluation->second;
    }
    m_evaluation = instantiate(node.get_variable());
  }

  inline std::shared_ptr<reactor_builder> reactor_translator::evaluate(
      const expression& e) {
    e.apply(*this);
    return m_evaluation;
  }

  inline std::shared_ptr<reactor_builder> reactor_translator::instantiate(
      std::shared_ptr<variable> v) {
    if(v->get_name() == "print") {
      auto print = m_scope->find<function>(v->get_name());
      for(auto overload : print->get_overloads()) {
        if(v == overload) {
          auto signature = std::static_pointer_cast<function_data_type>(
            overload->get_data_type());
          auto builder = [&] () -> std::shared_ptr<reactor_builder> {
            if(*signature->get_parameters()[0].m_type == bool_data_type()) {
              return make_ostream_reactor_builder<bool>(std::cout);
            } else if(*signature->get_parameters()[0].m_type ==
                float_data_type()) {
              return make_ostream_reactor_builder<double>(std::cout);
            } else if(*signature->get_parameters()[0].m_type ==
                integer_data_type()) {
              return make_ostream_reactor_builder<int>(std::cout);
            } else if(*signature->get_parameters()[0].m_type ==
                text_data_type()) {
              return make_ostream_reactor_builder<std::string>(std::cout);
            }

            // TODO: Print generic values
            return nullptr;
          }();
          m_variables[overload] = builder;
          return builder;
        }
      }
    } else if(v->get_name() == "chain") {
      auto chain = m_scope->find<function>(v->get_name());
      for(auto overload : chain->get_overloads()) {
        if(v == overload) {
          auto signature = std::static_pointer_cast<function_data_type>(
            overload->get_data_type());
          auto builder = [&] () -> std::shared_ptr<reactor_builder> {
            if(*signature->get_parameters()[0].m_type == bool_data_type()) {
              return make_chain_reactor_builder<bool>();
            } else if(*signature->get_parameters()[0].m_type ==
                float_data_type()) {
              return make_chain_reactor_builder<double>();
            } else if(*signature->get_parameters()[0].m_type ==
                integer_data_type()) {
              return make_chain_reactor_builder<int>();
            } else if(
                *signature->get_parameters()[0].m_type == text_data_type()) {
              return make_chain_reactor_builder<std::string>();
            }

            // TODO: Print generic values
            return nullptr;
          }();
          m_variables[overload] = builder;
          return builder;
        }
      }
    }
    return nullptr;
  }
}

#endif
