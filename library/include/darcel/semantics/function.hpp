#ifndef DARCEL_FUNCTION_HPP
#define DARCEL_FUNCTION_HPP
#include <vector>
#include "darcel/semantics/element.hpp"
#include "darcel/semantics/semantics.hpp"
#include "darcel/semantics/variable.hpp"

namespace darcel {

  //! Represents a potentially overloaded, named function.
  class function : public element {
    public:

      //! Constructs a function.
      /*!
        \param initial The initial overload.
      */
      function(std::shared_ptr<variable> initial);

      //! Returns the parent of this function.
      const std::shared_ptr<function>& get_parent() const;

      //! Returns the list of overloads.
      const std::vector<std::shared_ptr<variable>>& get_overloads() const;

      //! Adds an overload to this function.
      void add(std::shared_ptr<variable> overload);

      const location& get_location() const override final;

      const std::string& get_name() const override final;

    private:
      std::shared_ptr<function> m_parent;
      std::vector<std::shared_ptr<variable>> m_overloads;
  };

  //! Finds the function overload matching a set of parameters.
  /*!
    \param f The overloaded function to search over.
    \param parameters The parameter's to search.
    \return The variable representing the specific function overload.
  */
  inline std::shared_ptr<variable> find_overload(const function& f,
      const std::vector<function_data_type::parameter>& parameters) {
    for(auto& overload : f.get_overloads()) {
      auto type = std::static_pointer_cast<function_data_type>(
        overload->get_data_type());
      if(type->get_parameters().size() != parameters.size()) {
        continue;
      }
      auto is_match = [&] {
        for(std::size_t i = 0; i < parameters.size(); ++i) {
          if(*parameters[i].m_type != *type->get_parameters()[i].m_type) {
            return false;
          }
        }
        return true;
      }();
      if(is_match) {
        return overload;
      }
    }
    if(f.get_parent() != nullptr) {
      return find_overload(*f.get_parent(), parameters);
    }
    return nullptr;
  }

  inline function::function(std::shared_ptr<variable> initial) {
    m_overloads.push_back(std::move(initial));
  }

  inline const std::vector<std::shared_ptr<variable>>&
      function::get_overloads() const {
    return m_overloads;
  }

  inline const std::shared_ptr<function>& function::get_parent() const {
    return m_parent;
  }

  inline void function::add(std::shared_ptr<variable> overload) {
    m_overloads.push_back(std::move(overload));
  }

  inline const location& function::get_location() const {
    return m_overloads.front()->get_location();
  }

  inline const std::string& function::get_name() const {
    return m_overloads.front()->get_name();
  }
}

#endif
