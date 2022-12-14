/**
 * @file ast_visitor_test.cpp
 * @author Salvatore Cardamone
 * @brief Unit tests for the Kaleidoscope AST visitors.
 */
// clang-format off
#include <gtest/gtest.h>

#include "test/ast_test.hpp"
#include "hls/ast_visitor.hpp"
// clang-format on

/**
 * @brief ASTCodegen visitor Fixture for ASTs. Inherit all the ASTs
 * that are used in unit-testing the ASTs.
 */
class ASTCodegenTests : public ASTTests {
 protected:
  ASTCodegenTests() : ASTTests() {}
  ~ASTCodegenTests() {}
};

/**
 * @brief QQ
 */
TEST_F(ASTCodegenTests, Codegen) {
  std::string name("HLS");
  hls::ASTCodegen visitor(name);
  function_ast_.accept(visitor);
  std::cout << visitor << std::endl;
}
