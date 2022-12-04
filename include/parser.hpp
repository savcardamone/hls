/**
 * @file parser.hpp
 * @author Salvatore Cardamone
 * @brief Parser for the Kaleidoscope language.
 */
#ifndef __HLS_PARSER_HPP
#define __HLS_PARSER_HPP

#include <string>
#include <vector>

#include "lexer.hpp"

namespace hls {

/**
 * @brief Parser for the Kaleidoscope language. Wraps the lexer and constructs
 * the AST for the code.
 */
class Parser {
 public:
  Parser(std::istream& input) : lexer_{input} {
    // Prime the token buffer
    next_token();
  }

 private:
  Lexer lexer_;
  Token current_token_;
  std::map<char, int> binop_preecdence_{
      {'<', 10}, {'+', 20}, {'-', 20}, {'*', 40}};

  /**
   * @brief Retrieve the next Token from the lexer. This will be stored in
   * the current_token_ buffer.
   * @return The next Token from the lexer.
   */
  Token next_token() { return current_token_ = lexer_.get_token(); }

  // ==========================================================================
  //                        PRIMARY EXPRESSION PARSING
  // ==========================================================================

  /**
   * @brief Parse a primary expression of some kind; an identifier, number
   * or parenthetical expression.
   * @return The primary expression AST node.
   */
  std::unique_ptr<ast::ExprAST> parse_primary() {
    switch (current_token_.type()) {
      default:
        return nullptr;
      case TokenType::tok_identifier:
        return parse_identifier_expr();
      case TokenType::tok_number:
        return parse_number_expr();
      case TokenType::tok_operator:
        if (current_token_.value() == '(') return parse_parentheses_expr();
    }
  }

  /**
   * @brief Parse a numerical expression from the lexer and return the AST
   * node.
   * @return The numerical expression AST node.
   */
  std::unique_ptr<ast::ExprAST> parse_number_expr() {
    // Retrieve the numerical token in the token buffer and create an AST
    // node with it
    auto result = std::make_unique<ast::NumberExprAST>(
        strtof(current_token_.val().c_str()));
    next_token();
    return std::move(result);
  }

  /**
   * @brief Parse the contents from a parenthetical expression.
   * @return The expression AST node.
   */
  std::unique_ptr<ast::ExprAST> parse_parentheses_expr() {
    // Have already encountered '(', so move past it
    next_token();

    auto expr = parse_expression();
    if (!expr) return nullptr;

    if (current_token_.value() != ')') return nullptr;
    // Consume the trailing ')'
    next_token();

    return expr;
  }

  /**
   * @brief Parse some kind of identifier expression.
   *
   * This can take two forms;
   *    (1) Some kind of expression involving a variable, in which case
   *        return the variable, or;
   *    (2) Function call, in which case return the call AST node.
   * @return The identifier expression AST node.
   */
  std::unique_ptr<ast::ExprAST> parse_identifier_expr() {
    // Retrieve the identifier name, and move on to the rest of the
    // statement
    std::string name = current_token_.val();
    next_token();

    // If next token isn't an opening parenthesis, then we must be parsing
    // a basic variable expression rather than function call
    if (current_token_.val() != '(')
      return std::make_unique<ast::VariableExprAST>(name);

    // Otherwise we have a function call
    next_token();
    std::vector<std::unique_ptr<ast::ExprAST>> args;

    // While we haven't encountered the closing parenthesis, continue
    // parsing the expression
    if (current_token_.val() != ')') {
      while (1) {
        // Generic expression parsing and adding to the function argument
        // list
        if (auto arg = parse_expression())
          args.push_back(std::move(arg));
        else
          return nullptr;

        // End of call expression, so leave the loop
        if (current_token_.val() == ')') break;
        // Only permit comma operators separating identifiers
        if (current_token_.val() != ',') return nullptr;

        next_token();
      }
    }

    // Eat the closing ')' and return out call expression
    next_token();
    return std::make_unique<ast::CallExprAST>(name, std::move(args));
  }

  // ==========================================================================
  //                        BINARY EXPRESSION PARSING
  // ==========================================================================

  /**
   * @brief Evaluate the precedence of the Token in the token buffer.
   * @return Token precedence; if the token isn't a binary operator, return -1.
   */
  int get_token_precedence() {
    // Make sure the current token is actually an operator
    if (current_token_.type() != TokenType::tok_operator) return -1;
    // If we have an operator, establish precedence if it was a binary operator
    // Note that if a value isn't in the std::map, query will return 0
    return binop_precedence_[current_token_.value()] > 0
               ? binop_precedence_[current_token_.value()]
               : -1;
  }

  /**
   * @brief Parse an expression; a primary expression which is potentially
   * followed by a sequence of [binop, primary] pairs.
   *
   * For instance, consider a + (b + c) * d. This would be considered as the
   * sequence (a, [+, (b + c)], [*, d]) (recall that a parenthetical expression
   * is also a primary).
   * @return The expression AST node.
   */
  std::unique_ptr<ast::ExprAST> parse_expression() {
    auto lhs = parse_primary();
    if (!lhs) return nullptr;

    // Start off with a zero precedence since we've just parsed the first
    // primary in the expression
    return parse_binop_rhs(0, std::move(lhs));
  }

  /**
   * @brief Operator precedence parsing implementation for the parsing of
   * expressions. Effectively inserts parentheses around expressions with higher
   * precedences, turning them into primaries, allowing the expression to be
   * parsed recursively.
   * @param expr_precedence
   * @param lhs
   * @return
   */
  std::unique_ptr<ast::ExprAST> parse_binop_rhs(
      int expr_precedence, std::unique_ptr<ast::ExprAST> lhs) {
    // Keep on eating the full expression
    while (1) {
      // If we have a binop as the current token, then retrieve a precedence >=
      // 0
      int token_precedence = get_token_precedence();
      // If it was a binop that binds at least as tightly as the current binop,
      // then consume it, else we're done.
      // For instance, if our expression so far is a * b and we're parsing [+,
      // c],
      // + binds less tightly than * and so we return a * b as our
      if (token_precedence < expr_precedence) return lhs;

      // We know we've got a binary operator we need to process, so eat it
      Token binop = current_token_;
      next_token();

      // Try to retrieve the primary on the RHS of the binary operator; if there
      // isn't one there, there's something wrong (can't have a trailing binop)
      auto rhs = parse_primary();
      if (!rhs) return nullptr;

      // Look-ahead at the next binary operator in sequence. So far we've parsed
      // a `.` b `?`; if `?` is of lower precedence than `.`, e.g. the
      // expression looks like a * b + c, we want (a * b) + c, our primary being
      // a * b, so we can return LHS = a and RHS = b. We'll parse the [+, c]
      // later.
      //
      // However, if our expression is a + b * c, we want a + (b * c), so we
      // need to return LHS = a and RHS = (b * c). Hence if the look-ahead
      // operator is higher in precedence than the first, we need to recurse
      // into parse_binop_rhs with LHS = b, returning a binary expression AST of
      // (b * c). This can then be returned as the RHS to the LHS of a in the
      // first invocation of parse_binop_rhs
      int next_precedence = get_token_precedence();
      if (token_precedence < next_precedence) {
        rhs = parse_binop_rhs(token_precedence + 1, std::move(rhs));
        if (!rhs) return nullptr;
      }

      lhs = std::make_unique<ast::BinaryExprAST>(binop.value(), std::move(lhs),
                                                 std::move(rhs));
    }
  }

  /**
   * @brief Parse a function prototype of the form function_name(arg1, arg2,
   * ...).
   * @return Prototype AST node.
   */
  std::unique_ptr<ast::PrototypeAST> parse_prototype() {
    // Prototype must start with an identifier; eat the function name if so
    if (current_token_.type() != TokenType::tok_identifier) return nullptr;
    std::string function_name = current_token_.value();
    next_token();

    if (current_token_.value() != '(') return nullptr;

    // Eat up all arguments; note that these are not comma-delimited
    std::vector<std::string> arg_names;
    while (next_token().type() == TokenType::tok_identifier) {
      arg_names.push_back(current_token_.value());
    }

    if (current_token_.value() != ')') return nullptr;
    next_token();

    return std::make_unique<ast::PrototypeAST>(function_name,
                                               std::move(arg_names));
  }

  /**
   * @brief Parse a function definition, of the form
   * def func_name(arg1, arg2, ...) func_body. The result is a function AST node
   * comprising a prototype and function body.
   * @return Function definintion AST.
   */
  std::unique_ptr<ast::FunctionAST> parse_definition() {
    // Eat the def keyword
    next_token();

    // Parse the prototype following the def
    auto proto = parse_prototye();
    if (!proto) return nullptr;

    // Parse the function expression and return the function AST node if we've
    // been able to retrieve a valid expression
    if (auto expr = parse_expression()) {
      return std::make_unique<ast::FunctionAST>(std::move(proto),
                                                std::move(expr));
    }

    return nullptr;
  }

  /**
   * @brief Parse an external function declaration of the form
   * extern func_name(arg1, arg2, ...).
   * @return Prototype AST.
   */
  std::unique_ptr<ast::PrototypeAST> parse_extern() {
    // Eat the extern keywork
    next_token();
    return parse_prototype();
  }

  /**
   * @brief Parse the entry point to the application. Interpreted as an
   * anonymous function without name.
   * @return Function definition AST.
   */
  std::unique_ptr<ast::FunctionAST> parse_top_level() {
    if (auto expr = parse_expression()) {
      // Prototype is completely anonymous; no name or arguments
      auto proto =
          std::make_unique<ast::PrototypeAST>("", std::vector<std::string>());
      return std::make_unique<ast::FunctionAST>(std::move(proto),
                                                std::move(expr));
    }
    return nullptr;
  }
};

namespace ast {
/**
 * @brief Base class for all expression AST nodes.
 */
class ExprAST {
 public:
  /**
   * @brief Class destructor.
   */
  virtual ~ExprAST();
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
  BinaryExprAST(const char& op, std::unique_ptr<ExprAST> lhs,
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
              std::vector<std::unique_ptr<ExprAST>>& args)
      : callee_{callee}, args_{std::move(args)} {}

 private:
  std::string callee_;
  std::vector<std::unique_ptr<ExprAST>> args_;
}

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
  PrototypeAST(const std::string& name, std::vector<std::string>& args)
      : name_{name}, args_{std::move(args)} {}

  /**
   * @brief Getter for the function name.
   * @return The name of the function.
   */
  const std::string& name() const { return name_; }

 private:
  std::string name_;
  std::vector<std::string> args_;
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
  std::unique_ptr<PrototypeAST> proto_;
  std::unique_ptr<ExprAST> body_;
}

}  // namespace ast
}  // namespace hls

#endif /* #ifndef __HLS_PARSER_HPP */
