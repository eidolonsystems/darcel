#ifndef DARCEL_OPS_HPP
#define DARCEL_OPS_HPP
#include <memory>
#include <stdexcept>
#include <string>
#include "darcel/lexicon/operation.hpp"
#include "darcel/syntax/syntax.hpp"

namespace darcel {

  //! Enumerates all supported operators.
  enum class Op {

    //! (...
    OPEN_BRACKET,

    //! ...)
    CLOSE_BRACKET,

    //! +a
    PLUS,

    //! -a
    MINUS,

    //! a + b
    ADD,

    //! a - b
    SUBTRACT,

    //! a * b
    MULTIPLY,

    //! a / b
    DIVIDE
  };

  //! Specifies an operator's associativity.
  enum class Associativity {

    //! a + b + c => (a + b) + c
    LEFT_TO_RIGHT,

    //! a = b = c => a = (b = c)
    RIGHT_TO_LEFT
  };

  //! Returns the arity of an operation.
  /*!
    \param o The operation.
    \return The operation's arity.
  */
  constexpr int get_arity(Op o) {
    switch(o) {
      case Op::OPEN_BRACKET:
        return 1;
      case Op::CLOSE_BRACKET:
        return 1;
      case Op::PLUS:
        return 1;
      case Op::MINUS:
        return 1;
      case Op::ADD:
        return 2;
      case Op::SUBTRACT:
        return 2;
      case Op::MULTIPLY:
        return 2;
      case Op::DIVIDE:
        return 2;
    }
    throw std::runtime_error("Invalid operation specified.");
  }

  //! Returns an operation's precedence.
  /*!
    \param o The operation.
    \return The operation's precedence.
  */
  constexpr int get_precedence(Op o) {
    switch(o) {
      case Op::OPEN_BRACKET:
        return 3;
      case Op::CLOSE_BRACKET:
        return 3;
      case Op::PLUS:
        return 2;
      case Op::MINUS:
        return 2;
      case Op::ADD:
        return 0;
      case Op::SUBTRACT:
        return 0;
      case Op::MULTIPLY:
        return 1;
      case Op::DIVIDE:
        return 1;
    }
    throw std::runtime_error("Invalid operation specified.");
  }

  //! Returns an operation's associativity.
  /*!
    \param o The operation.
    \return The operation's associativity.
  */
  constexpr Associativity get_associativity(Op o) {
    switch(o) {
      case Op::OPEN_BRACKET:
        return Associativity::LEFT_TO_RIGHT;
      case Op::CLOSE_BRACKET:
        return Associativity::LEFT_TO_RIGHT;
      case Op::PLUS:
        return Associativity::RIGHT_TO_LEFT;
      case Op::MINUS:
        return Associativity::RIGHT_TO_LEFT;
      case Op::ADD:
        return Associativity::LEFT_TO_RIGHT;
      case Op::SUBTRACT:
        return Associativity::LEFT_TO_RIGHT;
      case Op::MULTIPLY:
        return Associativity::LEFT_TO_RIGHT;
      case Op::DIVIDE:
        return Associativity::LEFT_TO_RIGHT;
    }
    throw std::runtime_error("Invalid operation specified.");
  }

  //! Returns the unary operation represented by a token.
  /*!
    \param o The token representing the operation.
    \return The operation represented by the token.
  */
  inline Op get_unary_op(Operation o) {
    switch(o.get_symbol()) {
      case Operation::Symbol::PLUS:
        return Op::PLUS;
      case Operation::Symbol::MINUS:
        return Op::MINUS;
    }
    throw std::runtime_error("Invalid operation specified.");
  }

  //! Returns the binary operation represented by a token.
  /*!
    \param o The token representing the operation.
    \return The operation represented by the token.
  */
  inline Op get_binary_op(Operation o) {
    switch(o.get_symbol()) {
      case Operation::Symbol::PLUS:
        return Op::ADD;
      case Operation::Symbol::MINUS:
        return Op::SUBTRACT;
      case Operation::Symbol::TIMES:
        return Op::MULTIPLY;
      case Operation::Symbol::DIVIDE:
        return Op::DIVIDE;
    }
    throw std::runtime_error("Invalid operation specified.");
  }

  //! Returns an operation's function name.
  inline const std::string& get_function_name(Op o) {
    switch(o) {
      case Op::PLUS:
        {
          static std::string value = "plus";
          return value;
        }
      case Op::MINUS:
        {
          static std::string value = "minus";
          return value;
        }
      case Op::ADD:
        {
          static std::string value = "add";
          return value;
        }
      case Op::SUBTRACT:
        {
          static std::string value = "subtract";
          return value;
        }
      case Op::MULTIPLY:
        {
          static std::string value = "multiply";
          return value;
        }
      case Op::DIVIDE:
        {
          static std::string value = "divide";
          return value;
        }
    }
    throw std::runtime_error("Invalid operation specified.");
  }
}

#endif
