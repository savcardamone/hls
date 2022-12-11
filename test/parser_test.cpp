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
#include "hls/parser.hpp"
// clang-format on

class ParserTests : public ::testing::Test {
 protected:
  ParserTests()
      : function_input_{"def my_func(a)\n\r\t(a + b) * c"},
        function_lexer_{function_input_} {}
  ~ParserTests() {}

  std::stringstream function_input_;
  hls::Lexer function_lexer_;
};

TEST_F(ParserTests, TestQQ) {
  auto a = hls::BinaryExprAST('+', std::make_unique<hls::VariableExprAST>("a"),
                              std::make_unique<hls::VariableExprAST>("b"));
  //auto b = hls::BinaryExprAST('+', std::make_unique<hls::VariableExprAST>("a"),
  //                            std::make_unique<hls::VariableExprAST>("c"));
  auto b = hls::VariableExprAST("a");
  ASSERT_EQ(a, b);
}

TEST_F(ParserTests, TestFunctionDefinitionParsing) {
  hls::Parser parser(function_lexer_);
  auto result = parser.step();
  std::cout << *result << std::endl;
}
