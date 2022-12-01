/**
 * @file lexer_test.cpp
 * @author Salvatore Cardamone
 * @brief Test of the Kaleidoscope lexer.
 */
#include "lexer.hpp"

#include <gtest/gtest.h>

// Lexer works on istream, so we can create stringstream inputs to
// feed into the lexer for unit testing
std::stringstream prototype_input("def my_func()");
std::stringstream function_input("def my_func(a, b)\n\r\ta + b");

/**
 * @brief Really basic lexing of the prototype input. Should result
 * in the tokens ["def", "my_func", "(", ")", "std::nullopt"].
 */
TEST(LexerTest, TestPrototypeLexing) {
  hls::Lexer lexer;

  ASSERT_EQ(lexer.get_token(prototype_input),
            hls::Token(hls::TokenType::tok_def));
  ASSERT_EQ(lexer.get_token(prototype_input),
            hls::Token(hls::TokenType::tok_identifier, "my_func"));
  ASSERT_EQ(lexer.get_token(prototype_input),
            hls::Token(hls::TokenType::tok_operator, "("));
  ASSERT_EQ(lexer.get_token(prototype_input),
            hls::Token(hls::TokenType::tok_operator, ")"));
  ASSERT_EQ(lexer.get_token(prototype_input),
            hls::Token(hls::TokenType::tok_eof));
};

/**
 * @brief Lexing of the function input. Should result in the tokens
 * ["def", "my_func", "(", "a", ",", "b", ")", "a", "+", "b"].
 */
TEST(LexerTest, TestFunctionLexing) {
  hls::Lexer lexer;

  ASSERT_EQ(lexer.get_token(function_input),
            hls::Token(hls::TokenType::tok_def));
  ASSERT_EQ(lexer.get_token(function_input),
            hls::Token(hls::TokenType::tok_identifier, "my_func"));
  ASSERT_EQ(lexer.get_token(function_input),
            hls::Token(hls::TokenType::tok_operator, "("));
  ASSERT_EQ(lexer.get_token(function_input),
            hls::Token(hls::TokenType::tok_identifier, "a"));
  ASSERT_EQ(lexer.get_token(function_input),
            hls::Token(hls::TokenType::tok_operator, ","));
  ASSERT_EQ(lexer.get_token(function_input),
            hls::Token(hls::TokenType::tok_identifier, "b"));
  ASSERT_EQ(lexer.get_token(function_input),
            hls::Token(hls::TokenType::tok_operator, ")"));
  ASSERT_EQ(lexer.get_token(function_input),
            hls::Token(hls::TokenType::tok_identifier, "a"));
  ASSERT_EQ(lexer.get_token(function_input),
            hls::Token(hls::TokenType::tok_operator, "+"));
  ASSERT_EQ(lexer.get_token(function_input),
            hls::Token(hls::TokenType::tok_identifier, "b"));
  ASSERT_EQ(lexer.get_token(function_input),
            hls::Token(hls::TokenType::tok_eof));
};
