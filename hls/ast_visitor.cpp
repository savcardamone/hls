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

void ASTCodegen::number_expr(NumberExprAST& ast) {
  value_ = llvm::ConstantFP::get(*context_, llvm::APFloat(ast.value()));
  value_->print(llvm::errs());
}

void ASTCodegen::variable_expr(VariableExprAST& ast) {
  auto val = named_values_[ast.name()];
  value_ = val ? val : nullptr;
  value_->print(llvm::errs());
}

void ASTCodegen::binary_expr(BinaryExprAST& ast) {
  ast.lhs()->accept(*this);
  auto lhs = value_;
  ast.rhs()->accept(*this);
  auto rhs = value_;

  if (!(lhs || rhs)) value_ = nullptr;
  switch (ast.op()) {
    case '+': {
      value_ = builder_->CreateFAdd(lhs, rhs, "addtmp");
      break;
    }
    case '-': {
      value_ = builder_->CreateFSub(lhs, rhs, "subtmp");
      break;
    }
    case '*': {
      value_ = builder_->CreateFMul(lhs, rhs, "multmp");
      break;
    }
    case '<': {
      auto tmp_lhs = builder_->CreateFCmpULT(lhs, rhs, "cmptmp");
      value_ = builder_->CreateUIToFP(
          tmp_lhs, llvm::Type::getDoubleTy(*context_), "booltmp");
      break;
    }
    default:
      value_ = nullptr;
  }
  value_->print(llvm::errs());
}

void ASTCodegen::call_expr(CallExprAST& ast) {
  llvm::Function* callee = module_->getFunction(ast.callee());

  if (!callee) {
    value_ = nullptr;
    return;
  }
  if (callee->arg_size() != ast.args().size()) {
    value_ = nullptr;
    return;
  }

  std::vector<llvm::Value*> args;
  for (auto arg : ast.args()) {
    arg->accept(*this);
    args.push_back(value_);
  }

  value_ = builder_->CreateCall(callee, args, "calltmp");
}

void ASTCodegen::prototype(PrototypeAST& ast) {}

void ASTCodegen::function(FunctionAST& ast) {}

}  // namespace hls
