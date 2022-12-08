/**
 * @file parser.hpp
 * @author Salvatore Cardamone
 * @brief Parser for the Kaleidoscope language.
 */
#ifndef __HLS_PARSER_HPP
#define __HLS_PARSER_HPP

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "hls/ast.hpp"
#include "hls/lexer.hpp"

namespace hls {

/**
 * @brief Parser for the Kaleidoscope language. Wraps the lexer and constructs
 * the AST for the code.
 */
class Parser {
 public:
  /**
   * @brief Class constructor. Prime the token stream.
   * @param lexer Lexer which provides token stream.
   */
  Parser(Lexer& lexer) : lexer_{lexer} { next_token(); }

  /**
   * @brief Parse the token stream from the lexer until we encounter an EOF.
   */
  void parse() {
    // Prime the token buffer
    // Just continue looping
    while (current_token_.type() != TokenType::tok_eof) {
      step();
      next_token();
    }
  }

  /**
   * @brief Step through the token stream from the lexer until we can return a
   * complete AST node.
   * @return AST node that has been parsed.
   */
  std::unique_ptr<AST> step() {
    switch (current_token_.type()) {
      case TokenType::tok_eof:
        return nullptr;
      case TokenType::tok_def:
        return handle_definition();
      case TokenType::tok_extern:
        return handle_extern();
      case TokenType::tok_operator:
        if (current_token_.value() == ";") {
          next_token();
          break;
        }
      default:
        return handle_top_level();
    }
    return nullptr;
  }

 private:
  Lexer lexer_;
  Token current_token_;
  std::map<std::string, int> binop_precedence_{
      {"<", 10}, {"+", 20}, {"-", 20}, {"*", 40}};

  /**
   * @brief Parse an extern function declaration. Recovers from any internal
   * errors by ignoring erroneous parsing and moving onto next expression.
   */
  std::unique_ptr<AST> handle_extern() {
    if (auto result = parse_extern()) {
      std::cout << "Parsed extern." << std::endl;
      return result;
    } else {
      next_token();
      return nullptr;
    }
  }

  /**
   * @brief Parse a function definition. Recovers from any internal
   * errors by ignoring erroneous parsing and moving onto next expression.
   */
  std::unique_ptr<AST> handle_definition() {
    if (auto result = parse_definition()) {
      std::cout << "Parsed function definition." << std::endl;
      return result;
    } else {
      next_token();
      return nullptr;
    }
  }

  /**
   * @brief Parse a top-level definition. Recovers from any internal
   * errors by ignoring erroneous parsing and moving onto next expression.
   */
  std::unique_ptr<AST> handle_top_level() {
    if (auto result = parse_top_level()) {
      std::cout << "Parsed top-level." << std::endl;
      return result;
    } else {
      next_token();
      return nullptr;
    }
  }

  /**
   * @brief Erroneous parsing for expression.
   * @param message Message to print to stderr.
   * @return ExprAST unique pointer that just wraps nullptr.
   */
  std::unique_ptr<ExprAST> expr_error(const std::string& message) {
    std::cerr << message << std::endl;
    return nullptr;
  }

  /**
   * @brief Erroneous parsing for prototype.
   * @param message Message to print to stderr.
   * @return PrototypeAST unique pointer that just wraps nullptr.
   */
  std::unique_ptr<PrototypeAST> proto_error(const std::string& message) {
    expr_error(message);
    return nullptr;
  }

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
  std::unique_ptr<ExprAST> parse_primary() {
    switch (current_token_.type()) {
      default:
        return nullptr;
      case TokenType::tok_identifier:
        return parse_identifier_expr();
      case TokenType::tok_number:
        return parse_number_expr();
      case TokenType::tok_operator:
        if (current_token_.value() == "(") return parse_parentheses_expr();
    }
    return nullptr;
  }

  /**
   * @brief Parse a numerical expression from the lexer and return the AST
   * node.
   * @return The numerical expression AST node.
   */
  std::unique_ptr<ExprAST> parse_number_expr() {
    // Retrieve the numerical token in the token buffer and create an AST
    // node with it
    auto result =
        std::make_unique<NumberExprAST>(std::stof(current_token_.value()));
    next_token();
    return std::move(result);
  }

  /**
   * @brief Parse the contents from a parenthetical expression.
   * @return The expression AST node.
   */
  std::unique_ptr<ExprAST> parse_parentheses_expr() {
    // Have already encountered '(', so move past it
    next_token();

    auto expr = parse_expression();
    if (!expr)
      return expr_error(
          "Couldn't parse parentheses expression after ( character.");

    if (current_token_.value() != ")")
      return expr_error("No terminating ) character in parentheses expression");
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
  std::unique_ptr<ExprAST> parse_identifier_expr() {
    // Retrieve the identifier name, and move on to the rest of the
    // statement
    std::string name = current_token_.value();
    next_token();

    // If next token isn't an opening parenthesis, then we must be parsing
    // a basic variable expression rather than function call
    if (current_token_.value() != "(")
      return std::make_unique<VariableExprAST>(name);

    // Otherwise we have a function call
    next_token();
    std::vector<std::unique_ptr<ExprAST>> args;

    // While we haven't encountered the closing parenthesis, continue
    // parsing the expression
    if (current_token_.value() != ")") {
      while (1) {
        // Generic expression parsing and adding to the function argument
        // list
        if (auto arg = parse_expression())
          args.push_back(std::move(arg));
        else
          return expr_error("Unrecognised expression in function call.");

        // End of call expression, so leave the loop
        if (current_token_.value() == ")") break;
        // Only permit comma operators separating identifiers
        if (current_token_.value() != ",")
          return expr_error(
              "Only , character is permitted between function arguments.");

        next_token();
      }
    }

    // Eat the closing ')' and return out call expression
    next_token();
    return std::make_unique<CallExprAST>(name, std::move(args));
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
  std::unique_ptr<ExprAST> parse_expression() {
    auto lhs = parse_primary();
    if (!lhs) return expr_error("Couldn't parse LHS in expression.");

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
  std::unique_ptr<ExprAST> parse_binop_rhs(int expr_precedence,
                                           std::unique_ptr<ExprAST> lhs) {
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
      if (!rhs) return expr_error("Couldn't parse RHS in binop.");

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
        if (!rhs)
          return expr_error("Couldn't find RHS in recursive binop search.");
      }

      lhs = std::make_unique<BinaryExprAST>(*binop.value().c_str(),
                                            std::move(lhs), std::move(rhs));
    }
  }

  /**
   * @brief Parse a function prototype of the form function_name(arg1, arg2,
   * ...).
   * @return Prototype AST node.
   */
  std::unique_ptr<PrototypeAST> parse_prototype() {
    // Prototype must start with an identifier; eat the function name if so
    if (current_token_.type() != TokenType::tok_identifier)
      return proto_error("Prototype must begin with an identifier.");
    std::string function_name = current_token_.value();
    next_token();

    if (current_token_.value() != "(")
      return proto_error(
          "Prototype arguments must be separated from identifier by "
          "parenthesis.");

    // Eat up all arguments; note that these are not comma-delimited
    std::vector<std::string> arg_names;
    while (next_token().type() == TokenType::tok_identifier) {
      arg_names.push_back(current_token_.value());
    }

    if (current_token_.value() != ")")
      return proto_error("Prototype arguments must be ended with parenthesis.");
    next_token();

    return std::make_unique<PrototypeAST>(function_name, std::move(arg_names));
  }

  /**
   * @brief Parse a function definition, of the form
   * def func_name(arg1, arg2, ...) func_body. The result is a function AST node
   * comprising a prototype and function body.
   * @return Function definintion AST.
   */
  std::unique_ptr<FunctionAST> parse_definition() {
    // Eat the def keyword
    next_token();

    // Parse the prototype following the def
    auto proto = parse_prototype();
    if (!proto) return nullptr;

    // Parse the function expression and return the function AST node if we've
    // been able to retrieve a valid expression
    if (auto expr = parse_expression()) {
      return std::make_unique<FunctionAST>(std::move(proto), std::move(expr));
    }

    return nullptr;
  }

  /**
   * @brief Parse an external function declaration of the form
   * extern func_name(arg1, arg2, ...).
   * @return Prototype AST.
   */
  std::unique_ptr<PrototypeAST> parse_extern() {
    // Eat the extern keywork
    next_token();
    return parse_prototype();
  }

  /**
   * @brief Parse the entry point to the application. Interpreted as an
   * anonymous function without name.
   * @return Function definition AST.
   */
  std::unique_ptr<FunctionAST> parse_top_level() {
    if (auto expr = parse_expression()) {
      // Prototype is completely anonymous; no name or arguments
      auto proto =
          std::make_unique<PrototypeAST>("", std::vector<std::string>());
      return std::make_unique<FunctionAST>(std::move(proto), std::move(expr));
    }
    return nullptr;
  }
};

}  // namespace hls

#endif /* #ifndef __HLS_PARSER_HPP */
