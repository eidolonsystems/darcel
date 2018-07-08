#ifndef DARCEL_BUILTIN_TRANSLATOR_HPP
#define DARCEL_BUILTIN_TRANSLATOR_HPP
#include <iostream>
#include "darcel/reactors/chain_reactor.hpp"
#include "darcel/reactors/count_reactor.hpp"
#include "darcel/reactors/enum_to_string_reactor.hpp"
#include "darcel/reactors/first_reactor.hpp"
#include "darcel/reactors/fold_reactor.hpp"
#include "darcel/reactors/last_reactor.hpp"
#include "darcel/reactors/ostream_reactor.hpp"
#include "darcel/reactors/reactor_translator.hpp"
#include "darcel/reactors/tally_reactor.hpp"
#include "darcel/semantics/scope.hpp"

namespace darcel {

  //! Adds definitions for the builtin add functions.
  /*!
    \param translator The translator to add the definitions to.
    \param s The scope to find the function in.
  */
  inline void translate_add(reactor_translator& translator, const scope& s) {
    auto f = s.find<function>("add");
    if(f != nullptr) {
      for(auto& overload : s.get_definitions(*f)) {
        auto signature = std::static_pointer_cast<function_data_type>(
          overload->get_type());
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
    \param s The scope to find the function in.
  */
  inline void translate_chain(reactor_translator& translator, const scope& s) {
/*
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
        } else if(auto e = std::dynamic_pointer_cast<enum_data_type>(
            signature->get_parameters()[0].m_type)) {
          return make_chain_reactor_builder<int>();
        }

        // TODO: Chain generic values
        return nullptr;
      }
    };
    auto f = s.find<function>("chain");
    translator.add(f, f->get_overloads().back(), builder());
*/
  }

  //! Adds definitions for the builtin count function.
  /*!
    \param translator The translator to add the definitions to.
    \param s The scope to find the function in.
  */
  inline void translate_count(reactor_translator& translator, const scope& s) {
    auto f = s.find<function>("count");
    translator.add(s.get_definitions(*f).front(), make_count_builder());
  }

  //! Adds definitions for the builtin divide functions.
  /*!
    \param translator The translator to add the definitions to.
    \param s The scope to find the function in.
  */
  inline void translate_divide(reactor_translator& translator,
      const scope& s) {
    auto f = s.find<function>("divide");
    if(f != nullptr) {
      for(auto& overload : s.get_definitions(*f)) {
        auto signature = std::static_pointer_cast<function_data_type>(
          overload->get_type());
        if(signature->get_parameters().size() == 2) {
          if(*signature->get_parameters()[0].m_type == integer_data_type() &&
              *signature->get_parameters()[1].m_type == integer_data_type()) {
            translator.add(overload, make_divide_reactor_builder<int, int>());
          } else if(
              *signature->get_parameters()[0].m_type == float_data_type() &&
              *signature->get_parameters()[1].m_type == float_data_type()) {
            translator.add(overload,
              make_divide_reactor_builder<double, double>());
          }
        }
      }
    }
  }

  //! Adds definitions for the builtin first functions.
  /*!
    \param translator The translator to add the definitions to.
    \param s The scope to find the function in.
  */
  inline void translate_first(reactor_translator& translator, const scope& s) {
/*
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
        } else if(auto e = std::dynamic_pointer_cast<enum_data_type>(
            signature->get_parameters()[0].m_type)) {
          return make_first_reactor_builder<int>();
        }

        // TODO: Print generic values
        return nullptr;
      }
    };
    auto f = s.find<function>("first");
    translator.add(f, f->get_overloads().back(), builder());
*/
  }

  //! Adds definitions for the builtin fold functions.
  /*!
    \param translator The translator to add the definitions to.
    \param s The scope to find the function in.
  */
  inline void translate_fold(reactor_translator& translator, const scope& s) {
/*
    struct builder {
      std::unique_ptr<reactor_builder> operator ()(
          const std::shared_ptr<variable>& v) const {
        auto signature = std::static_pointer_cast<function_data_type>(
          v->get_data_type());
        if(*signature->get_parameters()[1].m_type == bool_data_type()) {
          return make_fold_reactor_builder<bool>();
        } else if(*signature->get_parameters()[1].m_type ==
            float_data_type()) {
          return make_fold_reactor_builder<double>();
        } else if(*signature->get_parameters()[1].m_type ==
            integer_data_type()) {
          return make_fold_reactor_builder<int>();
        } else if(
            *signature->get_parameters()[1].m_type == text_data_type()) {
          return make_fold_reactor_builder<std::string>();
        } else if(auto e = std::dynamic_pointer_cast<enum_data_type>(
            signature->get_parameters()[0].m_type)) {
          return make_fold_reactor_builder<int>();
        }

        // TODO: Fold generic values
        return nullptr;
      }
    };
    auto f = s.find<function>("fold");
    translator.add(f, f->get_overloads().back(), builder());
*/
  }

  //! Adds definitions for the builtin last functions.
  /*!
    \param translator The translator to add the definitions to.
    \param s The scope to find the function in.
  */
  inline void translate_last(reactor_translator& translator, const scope& s) {
/*
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
        } else if(auto e = std::dynamic_pointer_cast<enum_data_type>(
            signature->get_parameters()[0].m_type)) {
          return make_last_reactor_builder<int>();
        }

        // TODO: Print generic values
        return nullptr;
      }
    };
    auto f = s.find<function>("last");
    translator.add(f, f->get_overloads().back(), builder());
*/
  }

  //! Adds definitions for the builtin multiply functions.
  /*!
    \param translator The translator to add the definitions to.
    \param s The scope to find the function in.
  */
  inline void translate_multiply(reactor_translator& translator,
      const scope& s) {
    auto f = s.find<function>("multiply");
    if(f != nullptr) {
      for(auto& overload : s.get_definitions(*f)) {
        auto signature = std::static_pointer_cast<function_data_type>(
          overload->get_type());
        if(signature->get_parameters().size() == 2) {
          if(*signature->get_parameters()[0].m_type == integer_data_type() &&
              *signature->get_parameters()[1].m_type == integer_data_type()) {
            translator.add(overload, make_multiply_reactor_builder<int, int>());
          } else if(
              *signature->get_parameters()[0].m_type == float_data_type() &&
              *signature->get_parameters()[1].m_type == float_data_type()) {
            translator.add(overload,
              make_multiply_reactor_builder<double, double>());
          }
        }
      }
    }
  }

  //! Adds definitions for the builtin print functions.
  /*!
    \param translator The translator to add the definitions to.
    \param s The scope to find the function in.
  */
  inline void translate_print(reactor_translator& translator, const scope& s) {
    struct builder {
      std::unique_ptr<reactor_builder> operator ()(
          const std::shared_ptr<function_definition>& f) const {
        if(*f->get_type()->get_parameters()[0].m_type == bool_data_type()) {
          return make_ostream_reactor_builder<bool>(std::cout);
        } else if(*f->get_type()->get_parameters()[0].m_type ==
            float_data_type()) {
          return make_ostream_reactor_builder<double>(std::cout);
        } else if(*f->get_type()->get_parameters()[0].m_type ==
            integer_data_type()) {
          return make_ostream_reactor_builder<int>(std::cout);
        } else if(*f->get_type()->get_parameters()[0].m_type ==
            text_data_type()) {
          return make_ostream_reactor_builder<std::string>(std::cout);
        } else if(auto e = std::dynamic_pointer_cast<enum_data_type>(
            f->get_type()->get_parameters()[0].m_type)) {
          return std::make_unique<function_reactor_builder>(
            [=] (auto& parameters, auto& t) {
              return make_ostream_reactor(std::cout,
                std::static_pointer_cast<reactor<std::string>>(
                make_enum_to_string_reactor(e,
                std::static_pointer_cast<reactor<int>>(
                parameters.front()->build(t)))));
            });
        }

        // TODO: Print generic values
        return nullptr;
      }
    };
    auto f = s.find<function>("print");
    if(f != nullptr) {
      translator.add(s.get_definitions(*f).front(), builder());
    }
  }

  //! Adds definitions for the builtin subtract functions.
  /*!
    \param translator The translator to add the definitions to.
    \param s The scope to find the function in.
  */
  inline void translate_subtract(reactor_translator& translator,
      const scope& s) {
    auto f = s.find<function>("subtract");
    if(f != nullptr) {
      for(auto& overload : s.get_definitions(*f)) {
        auto signature = std::static_pointer_cast<function_data_type>(
          overload->get_type());
        if(signature->get_parameters().size() == 2) {
          if(*signature->get_parameters()[0].m_type == integer_data_type() &&
              *signature->get_parameters()[1].m_type == integer_data_type()) {
            translator.add(overload, make_subtract_reactor_builder<int, int>());
          } else if(
              *signature->get_parameters()[0].m_type == float_data_type() &&
              *signature->get_parameters()[1].m_type == float_data_type()) {
            translator.add(overload,
              make_subtract_reactor_builder<double, double>());
          }
        }
      }
    }
  }

  //! Adds definitions for the builtin tally functions.
  /*!
    \param translator The translator to add the definitions to.
    \param s The scope to find the function in.
  */
  inline void translate_tally(reactor_translator& translator, const scope& s) {
/*
    struct builder {
      std::unique_ptr<reactor_builder> operator ()(
          const std::shared_ptr<variable>& v) const {
        return make_tally_reactor_builder();
      }
    };
    auto f = s.find<function>("tally");
    translator.add(f, f->get_overloads().back(), builder());
*/
  }

  //! Adds definitions for all the builtin functions.
  /*!
    \param translator The translator to add the definitions to.
    \param s The scope to find the functions in.
  */
  inline void translate_builtins(reactor_translator& translator,
      const scope& s) {
    translate_add(translator, s);
    translate_chain(translator, s);
    translate_count(translator, s);
    translate_divide(translator, s);
    translate_first(translator, s);
    translate_fold(translator, s);
    translate_last(translator, s);
    translate_multiply(translator, s);
    translate_print(translator, s);
    translate_subtract(translator, s);
    translate_tally(translator, s);
  }
}

#endif
