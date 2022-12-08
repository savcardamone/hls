/**
 * @file ast.hpp
 * @author Salvatore Cardamone
 * @brief Abstract Syntax Tree node definitions for the Kaleidoscope languages.
 */
#ifndef __HLS_AST_HPP
#define __HLS_AST_HPP

#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include "hls/ast_visitor.hpp"

namespace hls {

class AST {
 public:
  /**
   * @brief Create string representation of object. This should be overridden by
   * anything deriving from ExprAST.
   * @return String representation of object.
   */
  virtual std::string print() const = 0;

  /**
   * @brief Visitor pattern to allow for algorithmic application across all AST
   * node types.
   * @param visitor The visitor to apply to the AST node.
   */
  virtual void accept(ASTVisitor& visitor) = 0;
};

/**
 * @brief Stream operator overload for output of ExprAST.
 * @param os The output stream.
 * @param ast The AST node to print.
 * @return The modified output stream.
 */
std::ostream& operator<<(std::ostream& os, const AST& ast) {
  os << ast.print();
  return os;
}

/**
 * @brief Base class for all expression AST nodes.
 */
class ExprAST : public AST {};

/**
 * @brief Numerical expression AST node for numeric literals.
 */
class NumberExprAST : public ExprAST {
 public:
  /**
   * @brief Class constructor.
   * @param val The numeric value to initialise the expression with.
   */
  NumberExprAST(const double& val) : val_{val} {}

  /**
   * @brief Overload of the string representation method for the object.
   * @return String representation of the object.
   */
  std::string print() const override {
    return "NumberExprAST: Value = " + std::to_string(val_);
  }

  /**
   * @brief Accept an ASTVisitor instance to manipulate the NumberExprAST
   * object.
   * @param visitor The visitor to apply to the AST node.
   */
  void accept(ASTVisitor& visitor) override { visitor.number_expr(this); }

 private:
  double val_;
};

/**
 * @brief Variable expression AST node for references to variables.
 */
class VariableExprAST : public ExprAST {
 public:
  /**
   * @brief Class constructor.
   * @param name Name of the variable.
   */
  VariableExprAST(const std::string& name) : name_{name} {}

  /**
   * @brief Overload of the string representation method for the object.
   * @return String representation of the object.
   */
  std::string print() const override {
    return "VariableExprAST: Name = " + name_;
  }

  /**
   * @brief Accept an ASTVisitor instance to manipulate the VariableExprAST
   * object.
   * @param visitor The visitor to apply to the AST node.
   */
  void accept(ASTVisitor& visitor) override { visitor.variable_expr(this); }

 private:
  std::string name_;
};

/**
 * @brief Expression AST node for binary operators of the form lhs * rhs,
 * where * is some binary operator.
 */
class BinaryExprAST : public ExprAST {
 public:
  /**
   * @brief Class constructor.
   * @param op The binary operator.
   * @param lhs Left-hand side expression from the binary expression.
   * @param rhs Right-hand side expression from the binary expression.
   */
  BinaryExprAST(const char op, std::unique_ptr<ExprAST> lhs,
                std::unique_ptr<ExprAST> rhs)
      : op_{op}, lhs_{std::move(lhs)}, rhs_{std::move(rhs)} {}

  /**
   * @brief Overload of the string representation method for the object.
   * @return String representation of the object.
   */
  std::string print() const override {
    return "BinaryExprAST: LHS = (" + lhs_->print() + "), Operator = " + op_ +
           ", RHS = (" + rhs_->print() + ")";
  }

  /**
   * @brief Accept an ASTVisitor instance to manipulate the BinaryExprAST
   * object.
   * @param visitor The visitor to apply to the AST node.
   */
  void accept(ASTVisitor& visitor) override { visitor.binary_expr(this); }

 private:
  char op_;
  std::unique_ptr<ExprAST> lhs_, rhs_;
};

/**
 * @brief Expression AST node for a function call of the form
 * callee(args), where callee is some identifier and args is a vector of
 * expression AST nodes.
 */
class CallExprAST : public ExprAST {
 public:
  /**
   * @brief Class constructor.
   * @param callee Identifier for the function name.
   * @param args Arguments to the function.
   */
  CallExprAST(const std::string& callee,
              std::vector<std::unique_ptr<ExprAST>> args)
      : callee_{callee}, args_{std::move(args)} {}

  /**
   * @brief Overload of the string representation method for the object.
   * @return String representation of the object.
   */
  std::string print() const override {
    std::string return_val;
    return_val += "CallExprAST: Signature = " + callee_ + "(";
    for (std::size_t idx = 0; idx < args_.size() - 1; ++idx) {
      return_val += args_[idx]->print() + ", ";
    }
    return_val += args_[args_.size() - 1]->print() + ")";
    return return_val;
  }

  /**
   * @brief Accept an ASTVisitor instance to manipulate the CallExprAST
   * object.
   * @param visitor The visitor to apply to the AST node.
   */
  void accept(ASTVisitor& visitor) override { visitor.call_expr(this); }

 private:
  std::string callee_;
  std::vector<std::unique_ptr<ExprAST>> args_;
};

/**
 * @brief AST node for a function prototype. This is much the same as the
 * CallExprAST, but here we're just detailing the function interface, i.e.
 * the names that it takes.
 */
class PrototypeAST : public AST {
 public:
  /**
   * @brief Class constructor.
   * @param name Name of the function.
   * @param args Vector containing argument names.
   */
  PrototypeAST(const std::string& name, std::vector<std::string> args)
      : name_{name}, args_{std::move(args)} {}

  /**
   * @brief Getter for the function name.
   * @return The name of the function.
   */
  const std::string& name() const { return name_; }

  /**
   * @brief Overload of the string representation method for the object.
   * @return String representation of the object.
   */
  std::string print() const override {
    std::string return_val;
    return_val += "PrototypeAST, Signature = " + name_ + "(";
    for (std::size_t idx = 0; idx < args_.size() - 1; ++idx)
      return_val += args_[idx] + ", ";
    return_val += args_[args_.size() - 1] + ")";
    return return_val;
  }

  /**
   * @brief Accept an ASTVisitor instance to manipulate the PrototypeAST
   * object.
   * @param visitor The visitor to apply to the AST node.
   */
  void accept(ASTVisitor& visitor) override { visitor.prototype(this); }

 private:
  std::string name_;
  std::vector<std::string> args_;
};

/**
 * @brief AST node for the function; both prototype and body, so the full
 * function definition..
 */
class FunctionAST : public AST {
 public:
  /**
   * @brief Class constructor.
   * @param proto Function prototype.
   * @param body Body of the function.
   */
  FunctionAST(std::unique_ptr<PrototypeAST> proto,
              std::unique_ptr<ExprAST> body)
      : proto_{std::move(proto)}, body_{std::move(body)} {}

  /**
   * @brief Overload of the string representation method for the object.
   * @return String representation of the object.
   */
  std::string print() const override {
    std::string return_val;
    return_val += "FunctionAST, Prototype = " + proto_->print() + ", " +
                  "Body = " + body_->print();
    return return_val;
  }

  /**
   * @brief Accept an ASTVisitor instance to manipulate the FunctionAST
   * object.
   * @param visitor The visitor to apply to the AST node.
   */
  void accept(ASTVisitor& visitor) override { visitor.function(this); }

 private:
  std::unique_ptr<PrototypeAST> proto_;
  std::unique_ptr<ExprAST> body_;
};

}  // namespace hls

#endif /* #ifndef __HLS_AST_HPP */
