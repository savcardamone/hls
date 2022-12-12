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
 * @brief Test fixture for parser unit tests. Initialisation of lexers
 * that can be used for parsing. Maybe should be using mock lexers for this?
 */
class ParserTests : public ::testing::Test {
 protected:
  /**
   * @brief Class constructor.
   */
  ParserTests()
      : function_input_{"def my_func(a b c)\n\r\t(a + b) * c"},
        function_lexer_{function_input_} {}

  /**
   * @brief Class destructor.
   */
  ~ParserTests() {}

  std::stringstream function_input_;
  hls::Lexer function_lexer_;
};

/**
 * @brief Verify that we can parse function definitions properly
 */
TEST_F(ParserTests, TestFunctionDefinitionParsing) {
  using namespace hls;

  Parser parser(function_lexer_);
  auto result = parser.step();

  // LHS of our overall binary expression is (a + b)
  auto parentheses = std::make_shared<BinaryExprAST>(
      '+', std::make_shared<VariableExprAST>("a"),
      std::make_shared<VariableExprAST>("b"));
  // Function body is binary expression with LHS (a + b) and RHS c
  auto func_body = std::make_shared<BinaryExprAST>(
      '*', parentheses, std::make_shared<VariableExprAST>("c"));
  auto prototype = std::make_shared<PrototypeAST>(
      "my_func", std::vector<std::string>{"a", "b", "c"});

  ASSERT_EQ(*result, FunctionAST(prototype, func_body));
};
