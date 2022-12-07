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

  /**
   * @brief Create string representation of object.
   * @return String representation of object.
   */
  virtual std::string print() const { return "Unreachable"; };
};

/**
 * @brief Stream operator overload for output of ExprAST.
 * @param os The output stream.
 * @param ast The AST node to print.
 * @return The modified output stream.
 */
std::ostream& operator<<(std::ostream& os, const ExprAST& ast) {
  os << ast.print();
  return os;
}

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
  virtual std::string print() const final {
    return "NumberExprAST: Value = " + std::to_string(val_);
  }

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
  virtual std::string print() const final {
    return "VariableExprAST: Name = " + name_;
  }

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
  virtual std::string print() const final {
    return "BinaryExprAST: LHS = (" + lhs_->print() + "), Operator = " + op_ +
           ", RHS = (" + rhs_->print() + ")";
  }

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
  virtual std::string print() const final {
    std::string return_val;
    return_val += "CallExprAST: Signature = " + callee_ + "(";
    for (int idx = 0; idx < args_.size() - 1; ++idx) {
      return_val += args_[idx]->print() + ", ";
    }
    return_val += args_[args_.size() - 1]->print() + ")";
    return return_val;
  }

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
  friend std::ostream& operator<<(std::ostream& os, const PrototypeAST& ast);
  std::string name_;
  std::vector<std::string> args_;
};

/**
 * @brief Stream operator overload for output of PrototypeAST.
 * @param os The output stream.
 * @param ast The AST node to print.
 * @return The modified output stream.
 */
std::ostream& operator<<(std::ostream& os, const PrototypeAST& ast) {
  os << "PrototypeAST, Signature = " << ast.name_ << "(";
  for (int idx = 0; idx < ast.args_.size() - 1; ++idx)
    os << ast.args_[idx] << ", ";
  os << ast.args_[ast.args_.size() - 1] << ")";
  return os;
}

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
  friend std::ostream& operator<<(std::ostream& os, const FunctionAST& ast);
  std::unique_ptr<PrototypeAST> proto_;
  std::unique_ptr<ExprAST> body_;
};

/**
 * @brief Stream operator overload for output of FunctionAST.
 * @param os The output stream.
 * @param ast The AST node to print.
 * @return The modified output stream.
 */
std::ostream& operator<<(std::ostream& os, const FunctionAST& ast) {
  os << "FunctionAST, Prototype = " << *ast.proto_ << ", "
     << "Body = " << *ast.body_;
  return os;
}

}  // namespace hls

#endif /* #ifndef __HLS_AST_HPP */
