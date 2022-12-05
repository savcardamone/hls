/**
 * @file ast.hpp
 * @author Salvatore Cardamone
 * @brief Abstract Syntax Tree node definitions for the Kaleidoscope languages.
 */
#ifndef __HLS_AST_HPP
#define __HLS_AST_HPP

#include <memory>
#include <string>
#include <vector>

namespace hls {

/**
 * @brief Base class for all expression AST nodes.
 */
class ExprAST {
 public:
  /**
   * @brief Class destructor.
   */
  ~ExprAST(){};
};

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

 private:
  std::string callee_;
  std::vector<std::unique_ptr<ExprAST>> args_;
};

/**
 * @brief AST node for a function prototype. This is much the same as the
 * CallExprAST, but here we're just detailing the function interface, i.e.
 * the names that it takes.
 */
class PrototypeAST {
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

 private:
  std::string name_;
  std::vector<std::string> args_;
};

/**
 * @brief AST node for the function; both prototype and body, so the full
 * function definition..
 */
class FunctionAST {
 public:
  /**
   * @brief Class constructor.
   * @param proto Function prototype.
   * @param body Body of the function.
   */
  FunctionAST(std::unique_ptr<PrototypeAST> proto,
              std::unique_ptr<ExprAST> body)
      : proto_{std::move(proto)}, body_{std::move(body)} {}

 private:
  std::unique_ptr<PrototypeAST> proto_;
  std::unique_ptr<ExprAST> body_;
};

}  // namespace hls

#endif /* #ifndef __HLS_AST_HPP */
