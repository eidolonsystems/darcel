#ifndef DARCEL_OVERLOADED_DATA_TYPE_HPP
#define DARCEL_OVERLOADED_DATA_TYPE_HPP
#include "darcel/data_types/data_type.hpp"
#include "darcel/data_types/data_type_visitor.hpp"
#include "darcel/data_types/data_types.hpp"
#include "darcel/data_types/function_data_type.hpp"

namespace darcel {

  //! Represents a data type used by overloaded function expressions.
  class overloaded_data_type : public data_type {
    public:

      //! Constructs an empty overloaded data type.
      /*!
        \param l The location of the first function definition.
        \param name The name of the function being overloaded.
      */
      overloaded_data_type(location l, std::string name);

      //! Returns the list of overloads.
      const std::vector<std::shared_ptr<data_type>>& get_overloads() const;

      //! Adds an overload.
      /*!
        \param overload The overloaded data type to add.
        \return true iff the overload was added, otherwise an existing overload
                with an equal signature already exists.
      */
      bool add(std::shared_ptr<data_type> overload);

      const location& get_location() const override final;

      const std::string& get_name() const override final;

      void apply(data_type_visitor& visitor) const override final;

    protected:
      bool is_equal(const data_type& rhs) const override final;

    private:
      location m_location;
      std::string m_name;
      std::vector<std::shared_ptr<data_type>> m_overloads;
  };

  //! Tests if an overloaded data type contains an overload.
  /*!
    \param t The overloaded data type.
    \param o The overload to find.
    \return true iff o is a member of t.
  */
  inline bool has_overload(const overloaded_data_type& t,
      const data_type& o) {
    auto f = dynamic_cast<const function_data_type*>(&o);
    if(f == nullptr) {
      return false;
    }
    for(auto& o : t.get_overloads()) {
      auto g = static_cast<const function_data_type*>(o.get());
      if(equal_signature(*f, *g)) {
        return true;
      }
    }
    return false;
  }

  inline overloaded_data_type::overloaded_data_type(location l,
      std::string name)
      : m_location(std::move(l)),
        m_name(std::move(name)) {}

  inline const std::vector<std::shared_ptr<data_type>>&
      overloaded_data_type::get_overloads() const {
    return m_overloads;
  }

  inline bool overloaded_data_type::add(std::shared_ptr<data_type> overload) {
    auto f = dynamic_cast<const function_data_type*>(overload.get());
    if(f == nullptr) {
      return false;
    }
    if(has_overload(*this, *f)) {
      return false;
    }
    m_overloads.push_back(std::move(overload));
    return true;
  }

  inline const location& overloaded_data_type::get_location() const {
    return m_location;
  }

  inline const std::string& overloaded_data_type::get_name() const {
    return m_name;
  }

  inline void overloaded_data_type::apply(data_type_visitor& visitor) const {
    visitor.visit(*this);
  }

  inline bool overloaded_data_type::is_equal(const data_type& rhs) const {
    auto& other = static_cast<const overloaded_data_type&>(rhs);
    if(m_overloads.size() != other.m_overloads.size()) {
      return false;
    }
    for(auto& o : m_overloads) {
      auto found = false;
      for(auto& p : other.m_overloads) {
        if(*o == *p) {
          found = true;
          break;
        }
      }
      if(!found) {
        return false;
      }
    }
    return true;
  }

  inline void data_type_visitor::visit(const overloaded_data_type& node) {
    visit(static_cast<const data_type&>(node));
  }
}

#endif
