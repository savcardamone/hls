/**
 * @file ast_visitor.cpp
 * @author Salvatore Cardamone
 * @brief Visitors for the AST node types.
 *
 * Need these outside of the header since the AST types are forward-declared in
 * there and we need to interact with their interfaces.
 */

// clang-format keeps on inserting a newline between these two headers for
// some reason
// clang-format off
#include "ast_visitor.hpp"
#include "ast.hpp"
// clang-format on

namespace hls {

ASTCodegen::ASTCodegen(std::string& name)
    : context_{std::make_unique<llvm::LLVMContext>()},
      builder_{std::make_unique<llvm::IRBuilder<>>(*context_)},
      module_{std::make_unique<llvm::Module>(name, *context_)} {}

void ASTCodegen::number_expr(NumberExprAST* ast) {
  auto val = llvm::ConstantFP::get(*context_, llvm::APFloat(ast->value()));
  // Temporary to stop the unused variable error
  val->print(llvm::errs());
}

void ASTCodegen::variable_expr(VariableExprAST*) {}

void ASTCodegen::binary_expr(BinaryExprAST*) {}

void ASTCodegen::call_expr(CallExprAST*) {}

void ASTCodegen::prototype(PrototypeAST*) {}

void ASTCodegen::function(FunctionAST*) {}

}  // namespace hls
