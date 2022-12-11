/**
 * @file ast_test.cpp
 * @author Salvatore Cardamone
 * @brief Unit tests for the Kaleidoscope ASTs.
 */
// clang-format off
#include <gtest/gtest.h>

#include "hls/ast.hpp"
// clang-format on

/**
 * @brief Test Fixture for the AST unit-testing.
 */
class ASTTests : public ::testing::Test {
 protected:
  /**
   * @brief Class constructor. Initialise all of the ASTs that we'll use during
   * unit testing.
   */
  ASTTests() {
    // ====================================================================
    //                 Setting up the number expressions
    // ====================================================================
    number_expr_ast_ = hls::NumberExprAST(1.0);
    number_expr_ast_a_ = hls::NumberExprAST(1.0);
    number_expr_ast_b_ = hls::NumberExprAST(2.0);

    // ====================================================================
    //                 Setting up the variable expressions
    // ====================================================================
    variable_expr_ast_ = hls::VariableExprAST("a");
    variable_expr_ast_a_ = hls::VariableExprAST("a");
    variable_expr_ast_b_ = hls::VariableExprAST("b");

    // ====================================================================
    //                 Setting up the binary expressions
    // ====================================================================
    binary_expr_ast_ =
        hls::BinaryExprAST('+', std::make_shared<hls::VariableExprAST>("a"),
                           std::make_shared<hls::NumberExprAST>(1.0));
    binary_expr_ast_a_ =
        hls::BinaryExprAST('+', std::make_shared<hls::VariableExprAST>("a"),
                           std::make_shared<hls::NumberExprAST>(1.0));
    binary_expr_ast_b_ =
        hls::BinaryExprAST('+', std::make_shared<hls::VariableExprAST>("b"),
                           std::make_shared<hls::NumberExprAST>(2.0));

    // ====================================================================
    //                 Setting up the call expressions
    // ====================================================================
    call_expr_ast_ = hls::CallExprAST(
        "my_func_a", {std::make_shared<hls::VariableExprAST>("a"),
                      std::make_shared<hls::NumberExprAST>(1.0)});
    call_expr_ast_a_ = hls::CallExprAST(
        "my_func_a", {std::make_shared<hls::VariableExprAST>("a"),
                      std::make_shared<hls::NumberExprAST>(1.0)});
    call_expr_ast_b_ = hls::CallExprAST(
        "my_func_a", {std::make_shared<hls::VariableExprAST>("c"),
                      std::make_shared<hls::NumberExprAST>(2.0),
                      std::make_shared<hls::VariableExprAST>("e")});

    // ====================================================================
    //                    Setting up the prototypes
    // ====================================================================
    prototype_ast_ = hls::PrototypeAST("my_func", {"a", "b"});
    prototype_ast_a_ = hls::PrototypeAST("my_func", {"a", "b"});
    prototype_ast_b_ = hls::PrototypeAST("my_func", {"c", "d"});

    // ====================================================================
    //                    Setting up the functions
    // ====================================================================
    function_ast_ =
        hls::FunctionAST(std::make_shared<hls::PrototypeAST>(
                             "my_func", std::vector<std::string>{"a", "b"}),
                         std::make_shared<hls::BinaryExprAST>(
                             '+', std::make_shared<hls::VariableExprAST>("a"),
                             std::make_shared<hls::VariableExprAST>("b")));
    function_ast_a_ =
        hls::FunctionAST(std::make_shared<hls::PrototypeAST>(
                             "my_func", std::vector<std::string>{"a", "b"}),
                         std::make_shared<hls::BinaryExprAST>(
                             '+', std::make_shared<hls::VariableExprAST>("a"),
                             std::make_shared<hls::VariableExprAST>("b")));
    function_ast_b_ =
        hls::FunctionAST(std::make_shared<hls::PrototypeAST>(
                             "my_func", std::vector<std::string>{"c", "d"}),
                         std::make_shared<hls::BinaryExprAST>(
                             '-', std::make_shared<hls::VariableExprAST>("c"),
                             std::make_shared<hls::VariableExprAST>("d")));
  }

  /**
   * @brief Class destructor.
   */
  ~ASTTests() {}

  hls::NumberExprAST number_expr_ast_, number_expr_ast_a_, number_expr_ast_b_;
  hls::VariableExprAST variable_expr_ast_, variable_expr_ast_a_,
      variable_expr_ast_b_;
  hls::BinaryExprAST binary_expr_ast_, binary_expr_ast_a_, binary_expr_ast_b_;
  hls::CallExprAST call_expr_ast_, call_expr_ast_a_, call_expr_ast_b_;
  hls::PrototypeAST prototype_ast_, prototype_ast_a_, prototype_ast_b_;
  hls::FunctionAST function_ast_, function_ast_a_, function_ast_b_;
};

/**
 * @brief Test equality conditions for number expressions.
 *
 * Verify self-equality, equality with equivalent object and inequality with
 * a different number expression.
 */
TEST_F(ASTTests, TestNumberExprEquality) {
  ASSERT_TRUE(number_expr_ast_ == number_expr_ast_);
  ASSERT_TRUE(number_expr_ast_ == number_expr_ast_a_);
  ASSERT_FALSE(number_expr_ast_ == number_expr_ast_b_);
}

/**
 * @brief Test inequality conditions for number expression with all other ASTs.
 */
TEST_F(ASTTests, TestNumberExprInequality) {
  ASSERT_FALSE(number_expr_ast_ == variable_expr_ast_);
  ASSERT_FALSE(number_expr_ast_ == binary_expr_ast_);
  ASSERT_FALSE(number_expr_ast_ == call_expr_ast_);
  ASSERT_FALSE(number_expr_ast_ == prototype_ast_);
  ASSERT_FALSE(number_expr_ast_ == function_ast_);
}

/**
 * @brief Test equality conditions for variable expressions.
 *
 * Verify self-equality, equality with equivalent object and inequality with
 * a different variable expression.
 */
TEST_F(ASTTests, TestVariableExprEquality) {
  ASSERT_TRUE(variable_expr_ast_ == variable_expr_ast_);
  ASSERT_TRUE(variable_expr_ast_ == variable_expr_ast_a_);
  ASSERT_FALSE(variable_expr_ast_ == variable_expr_ast_b_);
}

/**
 * @brief Test inequality conditions for variable expression with all other ASTs.
 */
TEST_F(ASTTests, TestVariableExprInequality) {
  ASSERT_FALSE(variable_expr_ast_ == number_expr_ast_);
  ASSERT_FALSE(variable_expr_ast_ == binary_expr_ast_);
  ASSERT_FALSE(variable_expr_ast_ == call_expr_ast_);
  ASSERT_FALSE(variable_expr_ast_ == prototype_ast_);
  ASSERT_FALSE(variable_expr_ast_ == function_ast_);
}

/**
 * @brief Test equality conditions for binary expressions.
 *
 * Verify self-equality, equality with equivalent object and inequality with
 * a different binary expression.
 */
TEST_F(ASTTests, TestBinaryExprEquality) {
  ASSERT_TRUE(binary_expr_ast_ == binary_expr_ast_);
  ASSERT_TRUE(binary_expr_ast_ == binary_expr_ast_a_);
  ASSERT_FALSE(binary_expr_ast_ == binary_expr_ast_b_);
}

/**
 * @brief Test inequality conditions for binary expression with all other ASTs.
 */
TEST_F(ASTTests, TestBinaryExprInequality) {
  ASSERT_FALSE(binary_expr_ast_ == number_expr_ast_);
  ASSERT_FALSE(binary_expr_ast_ == variable_expr_ast_);
  ASSERT_FALSE(binary_expr_ast_ == call_expr_ast_);
  ASSERT_FALSE(binary_expr_ast_ == prototype_ast_);
  ASSERT_FALSE(binary_expr_ast_ == function_ast_);
}

/**
 * @brief Test equality conditions for call expressions.
 *
 * Verify self-equality, equality with equivalent object and inequality with
 * a different call expression.
 */
TEST_F(ASTTests, TestCallExprEquality) {
  ASSERT_TRUE(call_expr_ast_ == call_expr_ast_);
  ASSERT_TRUE(call_expr_ast_ == call_expr_ast_a_);
  ASSERT_FALSE(call_expr_ast_ == call_expr_ast_b_);
}

/**
 * @brief Test inequality conditions for call expression with all other ASTs.
 */
TEST_F(ASTTests, TestCallExprInequality) {
  ASSERT_FALSE(call_expr_ast_ == number_expr_ast_);
  ASSERT_FALSE(call_expr_ast_ == variable_expr_ast_);
  ASSERT_FALSE(call_expr_ast_ == binary_expr_ast_);
  ASSERT_FALSE(call_expr_ast_ == prototype_ast_);
  ASSERT_FALSE(call_expr_ast_ == function_ast_);
}

/**
 * @brief Test equality conditions for prototypes.
 *
 * Verify self-equality, equality with equivalent object and inequality with
 * a different prototype.
 */
TEST_F(ASTTests, TestPrototypeEquality) {
  ASSERT_TRUE(prototype_ast_ == prototype_ast_);
  ASSERT_TRUE(prototype_ast_ == prototype_ast_a_);
  ASSERT_FALSE(prototype_ast_ == prototype_ast_b_);
}

/**
 * @brief Test inequality conditions for prototype with all other ASTs.
 */
TEST_F(ASTTests, TestPrototypeInequality) {
  ASSERT_FALSE(prototype_ast_ == number_expr_ast_);
  ASSERT_FALSE(prototype_ast_ == variable_expr_ast_);
  ASSERT_FALSE(prototype_ast_ == binary_expr_ast_);
  ASSERT_FALSE(prototype_ast_ == call_expr_ast_);
  ASSERT_FALSE(prototype_ast_ == function_ast_);
}

/**
 * @brief Test equality conditions for function.
 *
 * Verify self-equality, equality with equivalent object and inequality with
 * a different function.
 */
TEST_F(ASTTests, TestFunctionEquality) {
  ASSERT_TRUE(function_ast_ == function_ast_);
  ASSERT_TRUE(function_ast_ == function_ast_a_);
  ASSERT_FALSE(function_ast_ == function_ast_b_);
}

/**
 * @brief Test inequality conditions for function with all other ASTs.
 */
TEST_F(ASTTests, TestFunctionInequality) {
  ASSERT_FALSE(function_ast_ == number_expr_ast_);
  ASSERT_FALSE(function_ast_ == variable_expr_ast_);
  ASSERT_FALSE(function_ast_ == binary_expr_ast_);
  ASSERT_FALSE(function_ast_ == call_expr_ast_);
  ASSERT_FALSE(function_ast_ == prototype_ast_);
}
