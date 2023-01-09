/**
 * @file ast_visitor.hpp
 * @author Salvatore Cardamone
 * @brief Visitors for the AST node types.
 */
#ifndef __HLS_AST_VISITOR_HPP
#define __HLS_AST_VISITOR_HPP

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Pass.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>

#include <map>
#include <ostream>

namespace hls {

// Forward-declarations of all the AST nodes that our visitors need to
// manipulate
class ExprAST;
class NumberExprAST;
class VariableExprAST;
class BinaryExprAST;
class IfExprAST;
class ForExprAST;
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
  virtual void number_expr(NumberExprAST&) = 0;

  /**
   * @brief Manipulate a VariableExprAST node.
   */
  virtual void variable_expr(VariableExprAST&) = 0;

  /**
   * @brief Manipulate a BinaryExprAST node.
   */
  virtual void binary_expr(BinaryExprAST&) = 0;

  /**
   * @brief Manipulate a IfExprAST node.
   */
  virtual void if_expr(IfExprAST&) = 0;

  /**
   * @brief Manipulate a ForExprAST node.
   */
  virtual void for_expr(ForExprAST&) = 0;

  /**
   * @brief Manipulate a CallExprAST node.
   */
  virtual void call_expr(CallExprAST&) = 0;

  /**
   * @brief Manipulate a PrototypeAST node.
   */
  virtual void prototype(PrototypeAST&) = 0;

  /**
   * @brief Manipulate a FunctionAST node.
   */
  virtual void function(FunctionAST&) = 0;
};

class ASTCodegen : public ASTVisitor {
 public:
  /**
   * @brief Class constructor.
   * @param name Name of the IR module.
   * @param incremental_print Whether to incrementally print the IR generation
   * of each AST when processed. Default is false. Will be dumped to std::cerr.
   */
  ASTCodegen(std::string& name, bool incremental_print = false);

  /**
   * @brief Generate the LLVM IR for a NumberExprAST node.
   * @param ast The AST to generate IR for.
   */
  void number_expr(NumberExprAST& ast) override;

  /**
   * @brief Generate the LLVM IR for a VariableExprAST node.
   * @param ast The AST to generate IR for.
   */
  void variable_expr(VariableExprAST& ast) override;

  /**
   * @brief Generate the LLVM IR for a BinaryExprAST node.
   * @param ast The AST to generate IR for.
   */
  void binary_expr(BinaryExprAST& ast) override;

  /**
   * @brief Generate the LLVM IR for a IfExprAST node.
   * @param ast The AST to generate IR for.
   */
  void if_expr(IfExprAST& ast) override;

  /**
   * @brief Generate the LLVM IR for a ForExprAST node.
   * @param ast The AST to generate IR for.
   */
  void for_expr(ForExprAST& ast) override;

  /**
   * @brief Generate the LLVM IR for a CallExprAST node.
   * @param ast The AST to generate IR for.
   */
  void call_expr(CallExprAST& ast) override;

  /**
   * @brief Generate the LLVM IR for a PrototypeAST node.
   * @param ast The AST to generate IR for.
   */
  void prototype(PrototypeAST& ast) override;

  /**
   * @brief Generate the LLVM IR for a FunctionAST node.
   * @param ast The AST to generate IR for.
   */
  void function(FunctionAST& ast) override;

 private:
  bool incremental_print_;
  std::unique_ptr<llvm::LLVMContext> context_;
  std::unique_ptr<llvm::IRBuilder<>> builder_;
  std::unique_ptr<llvm::Module> module_;
  std::unique_ptr<llvm::legacy::FunctionPassManager> fpm_;
  std::map<std::string, llvm::Value*> named_values_;
  // Caches since the return type of a visitor must be void
  llvm::Value* value_;
  llvm::Function* function_;
  llvm::PHINode* phi_;
  friend std::ostream& operator<<(std::ostream& os, ASTCodegen& ast_codegen);

  /**
   * @brief Helper function to visit the AST and return the cached llvm::Value.
   * @param ast The AST to visit.
   * @return The cached llvm::Value.
   */
  llvm::Value* value(ExprAST& ast);

  /**
   * @brief Flush the llvm::Value cache and report an error.
   * @param msg The message to print to std::cerr.
   */
  void value_error(std::string&& msg) {
    value_ = nullptr;
    std::cerr << msg;
  }

  /**
   * @brief Flush the llvm::Value cache and report an error.
   * @param msg The message to print to std::cerr.
   */
  void function_error(std::string&& msg) {
    function_ = nullptr;
    std::cerr << msg;
  }
};

}  // namespace hls

#endif /* #ifndef __HLS_AST_VISITOR_HPP */
