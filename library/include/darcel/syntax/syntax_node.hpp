#ifndef DARCEL_SYNTAX_NODE_HPP
#define DARCEL_SYNTAX_NODE_HPP
#include "darcel/lexicon/location.hpp"
#include "darcel/semantics/semantics.hpp"
#include "darcel/syntax/syntax.hpp"

namespace darcel {

  //! The base class for a syntax node.
  class SyntaxNode {
    public:

      //! Constructs a SyntaxNode.
      /*!
        \param l The location of the syntax node.
      */
      SyntaxNode(Location l);

      virtual ~SyntaxNode() = default;

      //! Returns the location of the syntax node.
      const Location& get_location() const;

      //! Applies a SyntaxNodeVisitor to this instance.
      /*!
        \param visitor The visitor to apply.
      */
      virtual void apply(SyntaxNodeVisitor& visitor) const = 0;

    private:
      Location m_location;

      SyntaxNode(const SyntaxNode&) = delete;
      SyntaxNode& operator =(const SyntaxNode&) = delete;
  };

  inline SyntaxNode::SyntaxNode(Location l)
      : m_location(std::move(l)) {}

  inline const Location& SyntaxNode::get_location() const {
    return m_location;
  }
}

#endif
