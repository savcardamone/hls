#include "ast_visitor.hpp"

#include "ast.hpp"

namespace hls {

ASTCodegen::ASTCodegen(std::string& name)
    : context_{std::make_unique<llvm::LLVMContext>()} {}

void ASTCodegen::number_expr(NumberExprAST* ast) {
  auto val = llvm::ConstantFP::get(*context_, llvm::APFloat(ast->value()));
}

void ASTCodegen::variable_expr(VariableExprAST*) {}

void ASTCodegen::binary_expr(BinaryExprAST*) {}

void ASTCodegen::call_expr(CallExprAST*) {}

void ASTCodegen::prototype(PrototypeAST*) {}

void ASTCodegen::function(FunctionAST*) {}

}  // namespace hls
