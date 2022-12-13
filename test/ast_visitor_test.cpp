/**
 * @file ast_visitor_test.cpp
 * @author Salvatore Cardamone
 * @brief Unit tests for the Kaleidoscope AST visitors.
 */
// clang-format off
#include <gtest/gtest.h>

#include "hls/ast.hpp"
#include "hls/ast_visitor.hpp"
// clang-format on

TEST(ASTVisitorTests, Codegen) {

  auto expr = hls::NumberExprAST(3.14);
  std::string name("HLS");
  hls::ASTCodegen visitor(name);
  expr.accept(visitor);
  
}
