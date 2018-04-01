#ifndef DARCEL_BUILTIN_TRANSLATOR_HPP
#define DARCEL_BUILTIN_TRANSLATOR_HPP
#include <iostream>
#include "darcel/reactors/reactor_translator.hpp"
#include "darcel/semantics/scope.hpp"

namespace darcel {

  //! Adds definitions for the builtin print functions.
  /*!
    \param translator The translator to add the definitions to.
    \param s The scope to find the builtin add function in.
  */
  inline void translate_print(reactor_translator& translator, const scope& s) {
    struct print_translator {
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
    auto print = s.find<function>("print");
    translator.add(print->get_overloads().back(), print_translator());
  }

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

  //! Adds definitions for all the builtin functions.
  /*!
    \param translator The translator to add the definitions to.
    \param s The scope to find the builtin add function in.
  */
  inline void translate_builtins(reactor_translator& translator,
      const scope& s) {
    translate_add(translator, s);
    translate_print(translator, s);
  }
}

#endif
