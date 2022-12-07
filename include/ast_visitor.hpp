/**
 * @file ast_visitor.hpp
 * @author Salvatore Cardamone
 * @brief Visitors for the AST node types.
 */
#ifndef __HLS_AST_VISITOR_HPP
#define __HLS_AST_VISITOR_HPP

namespace hls {

// Forward-declarations of all the AST nodes that our visitors need to
// manipulate
class NumberExprAST;
class VariableExprAST;
class BinaryExprAST;
class CallExprAST;
class PrototypeAST;
class FunctionAST;

/**
 * @brief Visitor design pattern for visiting AST nodes. This is an abstract
 * interface.
 */
class ASTVisitor {
 public:
  /**
   * @brief Manipulate a NumberExprAST node.
   */
  virtual void number_expr(NumberExprAST*) = 0;

  /**
   * @brief Manipulate a VariableExprAST node.
   */
  virtual void variable_expr(VariableExprAST*) = 0;

  /**
   * @brief Manipulate a BinaryExprAST node.
   */
  virtual void binary_expr(BinaryExprAST*) = 0;

  /**
   * @brief Manipulate a CallExprAST node.
   */
  virtual void call_expr(CallExprAST*) = 0;

  /**
   * @brief Manipulate a PrototypeAST node.
   */
  virtual void prototype(PrototypeAST*) = 0;

  /**
   * @brief Manipulate a FunctionAST node.
   */
  virtual void function(FunctionAST*) = 0;
};

}  // namespace hls

#endif /* #ifndef __HLS_AST_VISITOR_HPP */
