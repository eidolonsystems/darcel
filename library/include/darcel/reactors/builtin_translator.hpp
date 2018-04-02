#ifndef DARCEL_BUILTIN_TRANSLATOR_HPP
#define DARCEL_BUILTIN_TRANSLATOR_HPP
#include <iostream>
#include "darcel/reactors/chain_reactor.hpp"
#include "darcel/reactors/first_reactor.hpp"
#include "darcel/reactors/last_reactor.hpp"
#include "darcel/reactors/ostream_reactor.hpp"
#include "darcel/reactors/reactor_translator.hpp"
#include "darcel/semantics/scope.hpp"

namespace darcel {

  //! Adds definitions for the builtin add functions.
  /*!
    \param translator The translator to add the definitions to.
    \param s The scope to find the builtin add function in.
  */
  inline void translate_add(reactor_translator& translator, const scope& s) {
    auto add = s.find<function>("add");
    if(add != nullptr) {
      for(auto& overload : add->get_overloads()) {
        auto signature = std::static_pointer_cast<function_data_type>(
          overload->get_data_type());
        if(signature->get_parameters().size() == 2) {
          if(*signature->get_parameters()[0].m_type == integer_data_type() &&
              *signature->get_parameters()[1].m_type == integer_data_type()) {
            translator.add(overload, make_add_reactor_builder<int, int>());
          } else if(
              *signature->get_parameters()[0].m_type == float_data_type() &&
              *signature->get_parameters()[1].m_type == float_data_type()) {
            translator.add(overload,
              make_add_reactor_builder<double, double>());
          } else if(
              *signature->get_parameters()[0].m_type == text_data_type() &&
              *signature->get_parameters()[1].m_type == text_data_type()) {
            translator.add(overload,
              make_add_reactor_builder<std::string, std::string>());
          }
        }
      }
    }
  }

  //! Adds definitions for the builtin chain functions.
  /*!
    \param translator The translator to add the definitions to.
    \param s The scope to find the builtin add function in.
  */
  inline void translate_chain(reactor_translator& translator, const scope& s) {
    struct builder {
      std::unique_ptr<reactor_builder> operator ()(
          const std::shared_ptr<variable>& v) const {
        auto signature = std::static_pointer_cast<function_data_type>(
          v->get_data_type());
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

        // TODO: Chain generic values
        return nullptr;
      }
    };
    auto f = s.find<function>("chain");
    translator.add(f, f->get_overloads().back(), builder());
  }

  //! Adds definitions for the builtin first functions.
  /*!
    \param translator The translator to add the definitions to.
    \param s The scope to find the builtin add function in.
  */
  inline void translate_first(reactor_translator& translator, const scope& s) {
    struct builder {
      std::unique_ptr<reactor_builder> operator ()(
          const std::shared_ptr<variable>& v) const {
        auto signature = std::static_pointer_cast<function_data_type>(
          v->get_data_type());
        if(*signature->get_parameters()[0].m_type == bool_data_type()) {
          return make_first_reactor_builder<bool>();
        } else if(*signature->get_parameters()[0].m_type ==
            float_data_type()) {
          return make_first_reactor_builder<float>();
        } else if(*signature->get_parameters()[0].m_type ==
            integer_data_type()) {
          return make_first_reactor_builder<int>();
        } else if(*signature->get_parameters()[0].m_type ==
            text_data_type()) {
          return make_first_reactor_builder<std::string>();
        }

        // TODO: Print generic values
        return nullptr;
      }
    };
    auto f = s.find<function>("first");
    translator.add(f, f->get_overloads().back(), builder());
  }

  //! Adds definitions for the builtin last functions.
  /*!
    \param translator The translator to add the definitions to.
    \param s The scope to find the builtin add function in.
  */
  inline void translate_last(reactor_translator& translator, const scope& s) {
    struct builder {
      std::unique_ptr<reactor_builder> operator ()(
          const std::shared_ptr<variable>& v) const {
        auto signature = std::static_pointer_cast<function_data_type>(
          v->get_data_type());
        if(*signature->get_parameters()[0].m_type == bool_data_type()) {
          return make_last_reactor_builder<bool>();
        } else if(*signature->get_parameters()[0].m_type ==
            float_data_type()) {
          return make_last_reactor_builder<float>();
        } else if(*signature->get_parameters()[0].m_type ==
            integer_data_type()) {
          return make_last_reactor_builder<int>();
        } else if(*signature->get_parameters()[0].m_type ==
            text_data_type()) {
          return make_last_reactor_builder<std::string>();
        }

        // TODO: Print generic values
        return nullptr;
      }
    };
    auto f = s.find<function>("last");
    translator.add(f, f->get_overloads().back(), builder());
  }

  //! Adds definitions for the builtin print functions.
  /*!
    \param translator The translator to add the definitions to.
    \param s The scope to find the builtin add function in.
  */
  inline void translate_print(reactor_translator& translator, const scope& s) {
    struct builder {
      std::unique_ptr<reactor_builder> operator ()(
          const std::shared_ptr<variable>& v) const {
        auto signature = std::static_pointer_cast<function_data_type>(
          v->get_data_type());
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
      }
    };
    auto f = s.find<function>("print");
    translator.add(f, f->get_overloads().back(), builder());
  }

  //! Adds definitions for all the builtin functions.
  /*!
    \param translator The translator to add the definitions to.
    \param s The scope to find the builtin add function in.
  */
  inline void translate_builtins(reactor_translator& translator,
      const scope& s) {
    translate_add(translator, s);
    translate_chain(translator, s);
    translate_first(translator, s);
    translate_last(translator, s);
    translate_print(translator, s);
  }
}

#endif
