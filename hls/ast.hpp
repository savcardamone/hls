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

#include "ast_visitor.hpp"

namespace hls {

/**
 * @brief Base class for all AST types allowing us to specify a common interface
 * for expression ASTs *as well as* prototype and function ASTs.
 */
class AST {
 public:
  /**
   * @brief Virtual destructor. Going to be referring to a lot of things by
   * pointer to base, and we need to be able to destroy the derived class
   * properly, hence we create a vtable entry for the destructor.
   */
  virtual ~AST() {}

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

  /**
   * @brief Operator overload for equality of AST objects.
   */
  virtual bool operator==(const AST& rhs) const = 0;
};

/**
 * @brief Stream operator overload for output of ExprAST.
 * @param os The output stream.
 * @param ast The AST node to print.
 * @return The modified output stream.
 */
static std::ostream& operator<<(std::ostream& os, const AST& ast) {
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
   * @brief Default constructor.
   */
  NumberExprAST() : val_{0} {}

  /**
   * @brief Class constructor.
   * @param val The numeric value to initialise the expression with.
   */
  NumberExprAST(const double& val) : val_{val} {}

  /**
   * @brief Getter for the underying value.
   * @return AST value.
   */
  double value() const { return val_; }

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
  void accept(ASTVisitor& visitor) override { visitor.number_expr(*this); }

  /**
   * @brief Equality overload.
   * @param rhs The RHS of the equality condition. If the LHS and RHS aren't
   * of the same type, then we catch the resultant exception and return false.
   * @return True if equal, false otherwise.
   */
  bool operator==(const AST& rhs) const override {
    try {
      return val_ == dynamic_cast<const NumberExprAST&>(rhs).val_;
    } catch (std::bad_cast&) {
      return false;
    }
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
   * @brief Default constructor.
   */
  VariableExprAST() : name_{""} {}

  /**
   * @brief Class constructor.
   * @param name Name of the variable.
   */
  VariableExprAST(const std::string& name) : name_{name} {}

  /**
   * @brief Getter for the underying name.
   * @return AST name.
   */
  std::string name() const { return name_; }

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
  void accept(ASTVisitor& visitor) override { visitor.variable_expr(*this); }

  /**
   * @brief Equality overload.
   * @param rhs The RHS of the equality condition. If the LHS and RHS aren't
   * of the same type, then we catch the resultant exception and return false.
   * @return True if equal, false otherwise.
   */
  bool operator==(const AST& rhs) const override {
    try {
      return name_ == dynamic_cast<const VariableExprAST&>(rhs).name_;
    } catch (std::bad_cast&) {
      return false;
    }
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
   * @brief Default constructor.
   */
  BinaryExprAST() : op_{' '}, lhs_{nullptr}, rhs_{nullptr} {}

  /**
   * @brief Class constructor.
   * @param op The binary operator.
   * @param lhs Left-hand side expression from the binary expression.
   * @param rhs Right-hand side expression from the binary expression.
   */
  BinaryExprAST(const char op, std::shared_ptr<ExprAST> lhs,
                std::shared_ptr<ExprAST> rhs)
      : op_{op}, lhs_{std::move(lhs)}, rhs_{std::move(rhs)} {}

  /**
   * @brief Getter for the underying operator.
   * @return AST operator.
   */
  char op() const { return op_; }

  /**
   * @brief Getter for the LHS expression.
   * @return LHS expression.
   */
  std::shared_ptr<ExprAST> lhs() const { return lhs_; }

  /**
   * @brief Getter for the RHS expression.
   * @return RHS expression.
   */
  std::shared_ptr<ExprAST> rhs() const { return rhs_; }

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
  void accept(ASTVisitor& visitor) override { visitor.binary_expr(*this); }

  /**
   * @brief Equality overload.
   *
   * Attempt to cast RHS to a BinaryExprAST and perform equality check. If we
   * get a bad cast, then the other AST isn't a BinaryExprAST, and consequently
   * not equal to the LHS.
   * @param rhs The RHS of the equality condition.
   * @return True if equal, false otherwise.
   */
  bool operator==(const AST& rhs) const override {
    try {
      auto recast_rhs = dynamic_cast<const BinaryExprAST&>(rhs);
      return *this == recast_rhs;
    } catch (std::bad_cast&) {
      return false;
    }
  }

  /**
   * @brief Equality overload for two BinaryExprAST objects.
   * @param The RHS BinaryExprAST of the equality condition.
   * @return True if equal, false otherwise.
   */
  bool operator==(const BinaryExprAST& rhs) const {
    return ((op_ == rhs.op_) && (*lhs_ == *rhs.lhs_) && (*rhs_ == *rhs.rhs_));
  }

 private:
  char op_;
  std::shared_ptr<ExprAST> lhs_, rhs_;
};

/**
 * @brief Expression AST node for a function call of the form
 * callee(args), where callee is some identifier and args is a vector of
 * expression AST nodes.
 */
class CallExprAST : public ExprAST {
 public:
  /**
   * @brief Default constructor.
   */
  CallExprAST() : callee_{""}, args_{} {}

  /**
   * @brief Class constructor.
   * @param callee Identifier for the function name.
   * @param args Arguments to the function.
   */
  CallExprAST(const std::string& callee,
              std::vector<std::shared_ptr<ExprAST>> args)
      : callee_{callee}, args_{std::move(args)} {}

  /**
   * @brief Getter for the callee.
   * @return Callee name.
   */
  std::string callee() const { return callee_; }

  /**
   * @brief Getter for the call arguments.
   * @return Arguments.
   */
  std::vector<std::shared_ptr<ExprAST>> args() const { return args_; }

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
  void accept(ASTVisitor& visitor) override { visitor.call_expr(*this); }

  /**
   * @brief Equality overload.
   *
   * Attempt to cast RHS to a CallExprAST and perform equality check. If we
   * get a bad cast, then the other AST isn't a CallExprAST, and consequently
   * not equal to the LHS.
   * @param rhs The RHS of the equality condition.
   * @return True if equal, false otherwise.
   */
  bool operator==(const AST& rhs) const override {
    try {
      auto recast_rhs = dynamic_cast<const CallExprAST&>(rhs);
      return *this == recast_rhs;
    } catch (std::bad_cast&) {
      return false;
    }
  }

  /**
   * @brief Equality overload for two CallExprAST objects.
   * @param The RHS CallExprAST of the equality condition.
   * @return True if equal, false otherwise.
   */
  bool operator==(const CallExprAST& rhs) const {
    bool result = callee_ == rhs.callee_;
    if (args_.size() != rhs.args_.size()) return false;
    for (std::size_t idx = 0; idx < args_.size(); ++idx)
      result &= (*args_[idx] == *rhs.args_[idx]);
    return result;
  }

 private:
  std::string callee_;
  std::vector<std::shared_ptr<ExprAST>> args_;
};

/**
 * @brief AST node for a function prototype. This is much the same as the
 * CallExprAST, but here we're just detailing the function interface, i.e.
 * the names that it takes.
 */
class PrototypeAST : public AST {
 public:
  /**
   * @brief Default constructor.
   */
  PrototypeAST() : name_{""}, args_{} {}

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
   * @brief Getter for the args.
   * @return Arguments.
   */
  std::vector<std::string> args() const { return args_; }

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
  void accept(ASTVisitor& visitor) override { visitor.prototype(*this); }

  /**
   * @brief Equality overload.
   * @param rhs The RHS of the equality condition. If the LHS and RHS aren't
   * of the same type, then we catch the resultant exception and return false.
   * @return True if equal, false otherwise.
   */
  bool operator==(const AST& rhs) const override {
    try {
      auto rhs_recast = dynamic_cast<const PrototypeAST&>(rhs);
      if (args_.size() != rhs_recast.args_.size()) return false;

      bool result = name_ == rhs_recast.name_;
      for (std::size_t idx = 0; idx < args_.size(); ++idx)
        result &= args_[idx] == rhs_recast.args_[idx];
      return result;
    } catch (std::bad_cast&) {
      return false;
    }
  }

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
   * @brief Default constructor.
   */
  FunctionAST() : proto_{nullptr}, body_{nullptr} {}

  /**
   * @brief Class constructor.
   * @param proto Function prototype.
   * @param body Body of the function.
   */
  FunctionAST(std::shared_ptr<PrototypeAST> proto,
              std::shared_ptr<ExprAST> body)
      : proto_{std::move(proto)}, body_{std::move(body)} {}

  /**
   * @brief Getter for the prototype.
   * @return Prototype.
   */
  std::shared_ptr<PrototypeAST> proto() const { return proto_; }

  /**
   * @brief Getter for the function body.
   * @return Function body.
   */
  std::shared_ptr<ExprAST> body() const { return body_; }

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
  void accept(ASTVisitor& visitor) override { visitor.function(*this); }

  /**
   * @brief Equality overload.
   *
   * Attempt to cast RHS to a FunctionAST and perform equality check. If we
   * get a bad cast, then the other AST isn't a FunctionAST, and consequently
   * not equal to the LHS.
   * @param rhs The RHS of the equality condition.
   * @return True if equal, false otherwise.
   */
  bool operator==(const AST& rhs) const override {
    try {
      auto recast_rhs = dynamic_cast<const FunctionAST&>(rhs);
      return *this == recast_rhs;
    } catch (std::bad_cast&) {
      return false;
    }
  }

  /**
   * @brief Equality overload for two FunctionAST objects.
   * @param The RHS FunctionAST of the equality condition.
   * @return True if equal, false otherwise.
   */
  bool operator==(const FunctionAST& rhs) const {
    return (*proto_ == *rhs.proto_) && (*body_ == *rhs.body_);
  }

 private:
  std::shared_ptr<PrototypeAST> proto_;
  std::shared_ptr<ExprAST> body_;
};

}  // namespace hls

#endif /* #ifndef __HLS_AST_HPP */
