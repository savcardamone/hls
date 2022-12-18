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
#include <iostream>
#include "ast_visitor.hpp"
#include "ast.hpp"
// clang-format on

namespace hls {

llvm::Value* ASTCodegen::value(ExprAST& ast) {
  ast.accept(*this);
  return value_;
}

std::ostream& operator<<(std::ostream& os, ASTCodegen& ast_codegen) {
  ast_codegen.module_->print(llvm::outs(), nullptr);
  return os;
}

ASTCodegen::ASTCodegen(std::string& name, bool incremental_print)
    : incremental_print_{incremental_print},
      context_{std::make_unique<llvm::LLVMContext>()},
      builder_{std::make_unique<llvm::IRBuilder<>>(*context_)},
      module_{std::make_unique<llvm::Module>(name, *context_)},
      fpm_{std::make_unique<llvm::legacy::FunctionPassManager>(module_.get())} {
  // Initialise the function pass manager to enable optimisations; you can find
  // these listed at https://llvm.org/docs/Passes.html. As the name suggests,
  // these optimisations are run per-function as opposed to the entire IR

  // Instruction-combining pass to create simpler expressions, e.g.
  // (y = x + 1); (z = y + 1) => (z = x + 2)
  fpm_->add(llvm::createInstructionCombiningPass());

  // Reassociate expressions to facilitate better constant propagation, etc.,
  // e.g. 4 + (x + 5) => x + (4 + 5) Different expression types are ranked
  // differently to do the reassociation, e.g. constants have rank 0, function
  // calls rank 1, etc.
  fpm_->add(llvm::createReassociatePass());

  // Global-Value numbering pass eliminates redundant instructions
  fpm_->add(llvm::createGVNPass());

  // Control-Flow Graph simplication removes dead code and merges basic blocks
  fpm_->add(llvm::createCFGSimplificationPass());
  fpm_->doInitialization();
}

void ASTCodegen::number_expr(NumberExprAST& ast) {
  // Constant numerical expressions are uniqued together in the
  // LLVM context
  value_ = llvm::ConstantFP::get(*context_, llvm::APFloat(ast.value()));
  if (incremental_print_) {
    value_->print(llvm::errs());
    std::cout << std::endl;
  }
}

void ASTCodegen::variable_expr(VariableExprAST& ast) {
  // Lookup whether the variable exists or not in the symbol table;
  // if it's not there, we return a nullptr
  auto val = named_values_[ast.name()];
  value_ = val ? val : nullptr;
  if (!value_) std::cerr << "Variable not in symbol table.\n";
  if (incremental_print_) {
    value_->print(llvm::errs());
    std::cout << std::endl;
  }
}

void ASTCodegen::binary_expr(BinaryExprAST& ast) {
  // Need to retrieve the LHS and RHS codegen from the ASTCodegen value
  // cache one at a time
  llvm::Value* lhs = value(*ast.lhs());
  llvm::Value* rhs = value(*ast.lhs());

  if (!(lhs || rhs)) {
    return value_error("Neither LHS nor RHS could be found.\n");
  }
  // Create the appropriate IR depending on the binary operator
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
      // Kaleidoscope only works in doubles, so convert the unsigned
      // long from the previous comparison into floating point
      value_ = builder_->CreateUIToFP(
          tmp_lhs, llvm::Type::getDoubleTy(*context_), "booltmp");
      break;
    }
    default: {
      return value_error("Unrecognised binary operator.\n");
    }
  }
  if (incremental_print_) {
    value_->print(llvm::errs());
    std::cout << std::endl;
  }
}

void ASTCodegen::if_expr(IfExprAST& ast) {
  // First of all we generate the IR for the condition of the if expression
  llvm::Value* cond = value(*ast.cond());
  if (!cond) {
    return value_error("Couldn't generate IR for if-condition.\n");
  }

  // Check whether condition is not-equal to zero
  cond = builder_->CreateFCmpONE(
      cond, llvm::ConstantFP::get(*context_, llvm::APFloat(0.0)), "ifcond");

  // Get the function that we're evaluating this control flow in
  llvm::Function* function = builder_->GetInsertBlock()->getParent();

  // Create basic blocks for the two paths; insert "then" into the function
  llvm::BasicBlock* then_bb =
      llvm::BasicBlock::Create(*context_, "then", function);
  llvm::BasicBlock* else_bb = llvm::BasicBlock::Create(*context_, "else");
  // Create basic block for the merged path once then/else has completed;
  // this will have the phi node in it
  llvm::BasicBlock* merge_bb = llvm::BasicBlock::Create(*context_, "ifcont");

  // Branch on the condition instruction
  builder_->CreateCondBr(cond, then_bb, else_bb);

  // =================================================================
  //                           THEN block
  // =================================================================

  // Set our cursor to just after the "then" basic block label
  builder_->SetInsertPoint(then_bb);
  // Then block codegen
  llvm::Value* then_expr = value(*ast.then_expr());
  if (!then_expr) {
    return value_error("Couldn't generate IR for then expression.\n");
  }
  // Unconditional branch to the merge block at the end of the "then"
  builder_->CreateBr(merge_bb);
  // Phi needs entry point for the "then" block when we set it up, so just
  // keep this as a reference for later
  then_bb = builder_->GetInsertBlock();

  // =================================================================
  //                           ELSE block
  // =================================================================

  // Insert the "else" basic block label into the function
  function->getBasicBlockList().push_back(else_bb);
  // Set our "cursor" to just after then "else" basic block label
  builder_->SetInsertPoint(else_bb);
  // Else block codegen
  llvm::Value* else_expr = value(*ast.else_expr());
  if (!else_expr) {
    return value_error("Couldn't generate IR for else expression.\n");
  }
  // Unconditional branch to the merge block at the end of the "else"
  builder_->CreateBr(merge_bb);
  // Phi needs entry point for the "else" block when we set it up, so
  // just keep this as a reference for later
  else_bb = builder_->GetInsertBlock();

  // =================================================================
  //                           PHI block
  // =================================================================

  // Insert the "ifcont" merging basic block label into the function
  function->getBasicBlockList().push_back(merge_bb);
  // Set our "cursor" to just after the "ifcont" basic block label
  builder_->SetInsertPoint(merge_bb);
  // Create the phi node with two incoming edges
  llvm::PHINode* phi_node =
      builder_->CreatePHI(llvm::Type::getDoubleTy(*context_), 2, "iftmp");
  // Add the edges to the phi node
  phi_node->addIncoming(then_expr, then_bb);
  phi_node->addIncoming(else_expr, else_bb);
  // Cache the phi
  phi_ = phi_node;
}

void ASTCodegen::call_expr(CallExprAST& ast) {
  // Check whether the function name exists or not in our symbol table
  // (should already be there from function definition or extern)
  llvm::Function* callee = module_->getFunction(ast.callee());
  if (!callee) {
    return function_error("Function was not found in symbol table.\n");
  }
  if (callee->arg_size() != ast.args().size()) {
    return function_error(
        "Number of arguments in CallExprAST does not match those in "
        "symbol table.\n");
  }

  std::vector<llvm::Value*> args;
  for (auto arg : ast.args()) {
    arg->accept(*this);
    args.push_back(value_);
  }

  value_ = builder_->CreateCall(callee, args, "calltmp");
  if (incremental_print_) {
    value_->print(llvm::errs());
    std::cout << std::endl;
  }
}

void ASTCodegen::prototype(PrototypeAST& ast) {
  // Get the function argument types
  std::vector<llvm::Type*> func_arg_types(ast.args().size(),
                                          llvm::Type::getDoubleTy(*context_));
  // Get the function type signature; note that the first argument here is
  // the return type, then come the argument types, then comes an indicator
  // that there are no variadic arguments
  llvm::FunctionType* func_type = llvm::FunctionType::get(
      llvm::Type::getDoubleTy(*context_), func_arg_types, false);

  // Create the IR function corresponding to the prototype; store the function
  // in the function cache of this object
  function_ = llvm::Function::Create(func_type, llvm::Function::ExternalLinkage,
                                     ast.name(), module_.get());

  // Set the argument names in the function signature
  unsigned int arg_idx = 0;
  for (auto& arg : function_->args()) arg.setName(ast.args()[arg_idx++]);

  if (incremental_print_) {
    function_->print(llvm::errs());
    std::cout << std::endl;
  }
}

void ASTCodegen::function(FunctionAST& ast) {
  // Check whether the function name is in the symbol table
  function_ = module_->getFunction(ast.proto()->name());

  // If not, then we need to do the codegen for the prototype (this is a
  // function definition)
  if (!function_) ast.proto()->accept(*this);

  // Function shouldn't have been defined yet if we've gotten this far; we
  // can't redefine
  if (!function_->empty()) {
    return function_error("Function redefinition.\n");
  }

  // Create the function basic block
  llvm::BasicBlock* bb =
      llvm::BasicBlock::Create(*context_, "entry", function_);
  builder_->SetInsertPoint(bb);

  // Clear out the list of in-scope variables
  named_values_.clear();
  for (auto& arg : function_->args())
    named_values_[arg.getName().data()] = &arg;

  ast.body()->accept(*this);
  if (value_) {
    builder_->CreateRet(value_);
    llvm::verifyFunction(*function_);
    // Run the function-pass manager for optimisations we set up in the
    // class constructor
    fpm_->run(*function_);
    if (incremental_print_) {
      function_->print(llvm::errs());
      std::cout << std::endl;
    }
    return;
  }

  function_->eraseFromParent();
  function_ = nullptr;
  std::cerr << "Function body could not be built.\n";
}

}  // namespace hls
