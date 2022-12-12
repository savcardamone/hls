/**
 * @file parser_test.cpp
 * @author Salvatore Cardamone
 * @brief Unit tests for the Kaleidoscope parser.
 */

// Weird behaviour of clang-format where it places gtest dependency
// between the two local ones; guess the '/' character has an ASCII code
// that's after '.' (sorts in ASCIIbetical order)
// clang-format off
#include <gtest/gtest.h>

#include "hls/lexer.hpp"
#include "hls/ast.hpp"
#include "hls/parser.hpp"
// clang-format on

/**
 * @brief Verify that we can parse the top-level of the code properly.
 */
TEST(ParserTests, TestTopLevelParsing) {
  using namespace hls;

  std::stringstream toplevel_input("a + my_func(b, c);");
  Lexer toplevel_lexer(toplevel_input);
  Parser parser(toplevel_lexer);
  auto result = parser.step();

  auto func_args = std::vector<std::shared_ptr<ExprAST>>{
      std::make_shared<VariableExprAST>("b"),
      std::make_shared<VariableExprAST>("c")};
  auto func_call =
      std::make_shared<CallExprAST>("my_func", std::move(func_args));
  auto expr = std::make_shared<BinaryExprAST>(
      '+', std::make_shared<VariableExprAST>("a"), func_call);
  auto proto = std::make_shared<PrototypeAST>("", std::vector<std::string>());

  ASSERT_EQ(*result, FunctionAST(std::move(proto), std::move(expr)));
}

/**
 * @brief Verify that we can parse an extern properly.
 */
TEST(ParserTests, TestExternParsing) {
  using namespace hls;

  std::stringstream extern_input("extern my_func(a b);");
  Lexer extern_lexer(extern_input);
  Parser parser(extern_lexer);
  auto result = parser.step();

  auto proto = PrototypeAST("my_func", std::vector<std::string>{"a", "b"});
  ASSERT_EQ(*result, proto);
}

/**
 * @brief Verify that we can parse function definitions properly.
 */
TEST(ParserTests, TestFunctionDefinitionParsing) {
  using namespace hls;

  std::stringstream function_input_a("def my_func(a b c)\n\r\t(a + b) * c");
  Lexer function_lexer_a(function_input_a);
  Parser parser_a(function_lexer_a);
  auto result_a = parser_a.step();

  // LHS of our overall binary expression is (a + b)
  auto parentheses = std::make_shared<BinaryExprAST>(
      '+', std::make_shared<VariableExprAST>("a"),
      std::make_shared<VariableExprAST>("b"));
  // Function body is binary expression with LHS (a + b) and RHS c
  auto func_body_a = std::make_shared<BinaryExprAST>(
      '*', parentheses, std::make_shared<VariableExprAST>("c"));
  auto prototype_a = std::make_shared<PrototypeAST>(
      "my_func", std::vector<std::string>{"a", "b", "c"});

  ASSERT_EQ(*result_a, FunctionAST(prototype_a, func_body_a));

  std::stringstream function_input_b("def my_func(a b c)\n\r\ta + b * c");
  Lexer function_lexer_b(function_input_b);
  Parser parser_b(function_lexer_b);
  auto result_b = parser_b.step();

  // LHS of our overall binary expression is a
  auto lhs = std::make_shared<VariableExprAST>("a");
  // RHS of our overall binary expression is b * c
  auto rhs = std::make_shared<BinaryExprAST>(
      '*', std::make_shared<VariableExprAST>("b"),
      std::make_shared<VariableExprAST>("c"));

  auto func_body_b = std::make_shared<BinaryExprAST>('+', lhs, rhs);
  auto prototype_b = std::make_shared<PrototypeAST>(
      "my_func", std::vector<std::string>{"a", "b", "c"});

  ASSERT_EQ(*result_b, FunctionAST(prototype_b, func_body_b));
};
