/**
 * @file ast_test.hpp
 * @author Salvatore Cardamone
 * @brief Test fixture definition for all Kaleidoscope ASTs.
 */
#ifndef __HLS_AST_TEST_HPP
#define __HLS_AST_TEST_HPP

#include <gtest/gtest.h>

#include "hls/ast.hpp"

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

#endif /* #ifndef __HLAS_AST_TEST_HPP */
