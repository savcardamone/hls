/**
 * @file ast_visitor.hpp
 * @author Salvatore Cardamone
 * @brief Visitors for the AST node types.
 */
#ifndef __HLS_AST_VISITOR_HPP
#define __HLS_AST_VISITOR_HPP

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

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

class ASTCodegen : public ASTVisitor {
 public:
  ASTCodegen(std::string& name);

  /**
   * @brief Manipulate a NumberExprAST node.
   */
  void number_expr(NumberExprAST* ast) override;

  /**
   * @brief Manipulate a VariableExprAST node.
   */
  void variable_expr(VariableExprAST*) override;

  /**
   * @brief Manipulate a BinaryExprAST node.
   */
  void binary_expr(BinaryExprAST*) override;

  /**
   * @brief Manipulate a CallExprAST node.
   */
  void call_expr(CallExprAST*) override;

  /**
   * @brief Manipulate a PrototypeAST node.
   */
  void prototype(PrototypeAST*) override;

  /**
   * @brief Manipulate a FunctionAST node.
   */
  void function(FunctionAST*) override;

 private:
  std::unique_ptr<llvm::LLVMContext> context_;
  std::unique_ptr<llvm::IRBuilder<>> builder_;
  std::unique_ptr<llvm::Module> module_;
  // std::map<std::string, llvm::Value*> named_values_;
};

}  // namespace hls

#endif /* #ifndef __HLS_AST_VISITOR_HPP */
