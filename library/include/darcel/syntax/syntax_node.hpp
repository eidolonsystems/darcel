#ifndef DARCEL_SYNTAX_NODE_HPP
#define DARCEL_SYNTAX_NODE_HPP
#include "darcel/lexicon/location.hpp"
#include "darcel/syntax/syntax.hpp"

namespace darcel {

  //! The base class for a syntax node.
  class syntax_node {
    public:

      //! Constructs a syntax_node.
      /*!
        \param l The location of the syntax node.
      */
      syntax_node(location l);

      virtual ~syntax_node() = default;

      //! Returns the location of the syntax node.
      const location& get_location() const;

      //! Applies a syntax_node_visitor to this instance.
      /*!
        \param visitor The visitor to apply.
      */
      virtual void apply(syntax_node_visitor& visitor) const = 0;

    private:
      location m_location;

      syntax_node(const syntax_node&) = delete;
      syntax_node& operator =(const syntax_node&) = delete;
  };

  inline syntax_node::syntax_node(location l)
      : m_location(std::move(l)) {}

  inline const location& syntax_node::get_location() const {
    return m_location;
  }
}

#endif
