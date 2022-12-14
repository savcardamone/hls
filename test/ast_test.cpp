/**
 * @file ast_test.cpp
 * @author Salvatore Cardamone
 * @brief Unit tests for the Kaleidoscope ASTs.
 */
// clang-format off
#include <gtest/gtest.h>

#include "test/ast_test.hpp"
// clang-format on

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
