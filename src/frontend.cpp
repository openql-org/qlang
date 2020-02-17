#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/ValueSymbolTable.h>
#include <llvm/IR/InlineAsm.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm-c/Core.h>
#include <llvm/IR/Attributes.h>
#include <string>
#include <sstream>
#include <stdarg.h>

#include "frontend.hpp"

using namespace qlang;


// TODO: just a test code.
llvm::Value* Frontend::qmeascall(QuantumRegister q1) {
  std::stringstream ss;
  ss << "qmeas.k  $0, " << q1 << ", qzero, 0";

  auto *funcType = llvm::FunctionType::get(llvm::Type::getInt64Ty(context), false);
  bool hasSideEffect = true;
  auto asmDialect = llvm::InlineAsm::AD_ATT;
  std::string constraints = "=r";
  auto *ia = llvm::InlineAsm::get(funcType, ss.str(), constraints, hasSideEffect, false, asmDialect);
  auto *result = builder.CreateCall(ia);
  result->addAttribute(llvm::AttributeList::FunctionIndex, llvm::Attribute::NoUnwind);
  return result;
}

void Frontend::qooxcall(QuantumRegister q1) {
  std::stringstream ss;
  ss << "qoox.k  qzero," << q1 << ", qzero, 1";

  auto *funcType = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
  bool hasSideEffect = true;
  auto asmDialect = llvm::InlineAsm::AD_ATT;
  std::string constraints = "";
  auto *ia = llvm::InlineAsm::get(funcType, ss.str(), constraints, hasSideEffect, false, asmDialect);
  auto *result = builder.CreateCall(ia);
  result->addAttribute(llvm::AttributeList::FunctionIndex, llvm::Attribute::NoUnwind);
}

void Frontend::telepcall(QuantumRegister q1, QuantumRegister q2) {
  std::stringstream ss;
  ss << "qtelep.k  qzero, " << q1 << ", " << q2 << ", 1";

  auto *funcType = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
  bool hasSideEffect = true;
  auto asmDialect = llvm::InlineAsm::AD_ATT;
  std::string constraints = "";
  auto *ia = llvm::InlineAsm::get(funcType, ss.str(), constraints, hasSideEffect, false, asmDialect);
  auto *result = builder.CreateCall(ia);
  result->addAttribute(llvm::AttributeList::FunctionIndex, llvm::Attribute::NoUnwind);
}


void Frontend::writeFunc(const char *format, llvm::Value *v = NULL) {
    auto *printfFunc = module->getFunction("printf");
    if (!printfFunc) {
        auto *Pty = llvm::PointerType::get(llvm::IntegerType::get(context, 8), 0);
        auto *FuncTy = llvm::FunctionType::get(llvm::IntegerType::get(context, 32), true);
        printfFunc = llvm::Function::Create(FuncTy, llvm::GlobalValue::ExternalLinkage, "printf", module);
        printfFunc->setCallingConv(llvm::CallingConv::C);
    }

    auto *str = builder.CreateGlobalStringPtr(format);
    std::vector <llvm::Value *> ArgsV;
    ArgsV.push_back(str);
    if (v)
      ArgsV.push_back(v);
/*
    va_list ap;
    va_start(ap, format);
    for (const char* p = format; *p != '\0'; ++p) {
        switch( *p ){
        case 'd':
            ArgsV.push_back(builder.getInt64(va_arg(ap, int)));
            break;
        case 'c':
            ArgsV.push_back(builder.getInt8(va_arg(ap, int)));
            break;
        case 's':
            ArgsV.push_back(builder.CreateGlobalStringPtr(va_arg(ap, char *)));
            break;
        default:
            break;
        }
    }
    va_end(ap);
*/
    builder.CreateCall(printfFunc, ArgsV);
}

Frontend::Frontend(const std::string &path)
    : lexer(path), context(), module(new llvm::Module("top", context)),
      builder(context) {
  cur_token = std::move(lexer.nextToken());
  peek_token = std::move(lexer.nextToken());
  {
    // std::vector<llvm::Type *> param_types(1, builder.getInt64Ty());
    // auto *funcType = llvm::FunctionType::get(builder.getInt64Ty(), param_types, false);
    auto *funcType = llvm::FunctionType::get(builder.getInt64Ty(), false);
    quantumFunc = llvm::Function::Create(
        funcType, llvm::Function::ExternalLinkage, "qfunc", module);
  }
}

void Frontend::compile() {
  block(nullptr);
  builder.CreateRet(builder.getInt64(0));
}

void Frontend::block(llvm::Function *func) {
  ident_table.enterBlock();
  std::vector<std::string> vars;
  std::vector<std::string> qints;

  while (true) {
    if (cur_token.type == TokenType::Const) {
      constDecl();
    } else if (cur_token.type == TokenType::Var) {
      varDecl(&vars);
    } else if (cur_token.type == TokenType::Qint) {
      qintDecl(&qints);
    } else if (cur_token.type == TokenType::Function) {
      functionDecl();
    } else {
      break;
    }
  }

  if (func != nullptr) {
    curFunc = func;
    builder.SetInsertPoint(&func->getEntryBlock());
    auto itr = func->arg_begin();
    for (size_t i = 0; i < func->arg_size(); i++) {
      auto *alloca =
          builder.CreateAlloca(builder.getInt64Ty(), 0, itr->getName());
      builder.CreateStore(itr, alloca);
      ident_table.appendVar(itr->getName().str(), alloca);
      itr++;
    }
    for (const auto &var : vars) {      
      auto *alloca = builder.CreateAlloca(builder.getInt64Ty(), 0, var);
      ident_table.appendVar(var, alloca);
    }
    statement();
    ident_table.leaveBlock();
  }
}

void Frontend::constDecl() {
  takeToken(TokenType::Const);
  while (true) {
    if (cur_token.type != TokenType::Ident) {
      parseError(TokenType::Ident, cur_token.type);
    }

    std::string const_name = cur_token.ident;
    nextToken();
    takeToken(TokenType::Equal);

    if (cur_token.type != TokenType::Integer) {
      parseError(TokenType::Integer, cur_token.type);
    }

    ident_table.appendConst(const_name, builder.getInt64(cur_token.integer));
    nextToken();

    if (cur_token.type == TokenType::Colon) {
      nextToken();
      // continue;
    } else if (cur_token.type == TokenType::Semicolon) {
      nextToken();
      break;
    } else {
      throw "unexpected at constDecl";
    }
  }
}

void Frontend::varDecl(std::vector<std::string> *vars) {
  takeToken(TokenType::Var);
  while (true) {
    if (cur_token.type != TokenType::Ident) {
      parseError(TokenType::Ident, cur_token.type);
    }

    vars->push_back(cur_token.ident);
    nextToken();

    if (cur_token.type == TokenType::Colon) {
      nextToken();
      // continue;
    } else if (cur_token.type == TokenType::Semicolon) {
      nextToken();
      break;
    } else {
      error("unexpected at varDecl");
    }
  }
}


void Frontend::qintDecl(std::vector<std::string> *qints) {
  takeToken(TokenType::Qint);
  while (true) {
    if (cur_token.type != TokenType::Ident) {
      parseError(TokenType::Ident, cur_token.type);
    }

    qints->push_back(cur_token.ident);
    nextToken();

    if (cur_token.type == TokenType::Colon) {
      nextToken();
      // continue;
    } else if (cur_token.type == TokenType::Semicolon) {
      nextToken();
      break;
    } else {
      error("unexpected at qintDecl");
    }
  }
}

void Frontend::functionDecl() {
  takeToken(TokenType::Function);

  if (cur_token.type == TokenType::Main) {
    auto *funcType = llvm::FunctionType::get(builder.getInt64Ty(), false);
    auto *mainFunc = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "main", module);
    auto *entry = llvm::BasicBlock::Create(context, "entrypoint", mainFunc);
    
    nextToken();

    std::vector<std::string> params;
    takeToken(TokenType::ParenL);
    while (true) {
      if (cur_token.type != TokenType::Ident) {
        break;
      }

      params.push_back(cur_token.ident);
      nextToken();
      if (cur_token.type == TokenType::Colon) {
        nextToken();
        // continue;
      } else {
        break;
      }
    }
    takeToken(TokenType::ParenR);

    auto itr = mainFunc->arg_begin();
    for (size_t i = 0; i < params.size(); i++) {
      ident_table.appendParam(params[i]);
      itr->setName(params[i]);
      itr++;
    }

    block(mainFunc);
  } else if (cur_token.type != TokenType::Ident) {
    parseError(TokenType::Ident, cur_token.type);
  } else {
    std::string func_name = std::move(cur_token.ident);
    nextToken();

    std::vector<std::string> params;
    takeToken(TokenType::ParenL);
    while (true) {
      if (cur_token.type != TokenType::Ident) {
        break;
      }

      params.push_back(cur_token.ident);
      nextToken();
      if (cur_token.type == TokenType::Colon) {
        nextToken();
        // continue;
      } else {
        break;
      }
    }
    takeToken(TokenType::ParenR);

    std::vector<llvm::Type *> param_types(params.size(), builder.getInt64Ty());
    auto *funcType =
        llvm::FunctionType::get(builder.getInt64Ty(), param_types, false);
    auto *func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage,
                                        func_name, module);
    auto *bblock = llvm::BasicBlock::Create(context, "entry", func);
    ident_table.appendFunction(func_name, func);

    auto itr = func->arg_begin();
    for (size_t i = 0; i < params.size(); i++) {
      ident_table.appendParam(params[i]);
      itr->setName(params[i]);
      itr++;
    }

    block(func);
  }
}

void Frontend::statement() {
  size_t backpatch_target;
  size_t start_at;
  std::vector<std::string> vars;
  std::vector<std::string> qints;

  llvm::Value *val;
  llvm::BasicBlock *stash;

  while (true) {
    if (cur_token.type == TokenType::Var) {
      varDecl(&vars);
    } else if (cur_token.type == TokenType::Qint) {
      qintDecl(&qints);
    } else {
      break;
    }
  }

  for (const auto &var : vars) {
    auto *alloca = builder.CreateAlloca(builder.getInt64Ty(), 0, var);
    ident_table.appendVar(var, alloca);
  }

  // TODO: alloc for just a qints registors.
  for (const auto &qint : qints) {
    QuantumRegister r;
    for (r = QuantumRegister::q1; r <= QuantumRegister::q31; ++r) {
      if (!qregmap[r]) {
        qregmap[r] = true;
        break;
      }
    }
    ident_table.appendQint(qint, r);
  }

  switch (cur_token.type) {
  case TokenType::Ident:
    statementAssign();
    return;
  case TokenType::CurlyBracesL:
    nextToken();    
    while (true) {
      statement();

      if (cur_token.type == TokenType::Semicolon) {
        takeToken(TokenType::Semicolon);
        continue;
      } else if (cur_token.type == TokenType::CurlyBracesR) {
        takeToken(TokenType::CurlyBracesR);
        break;
      } else {
        continue;
      }
    }
    return;
  case TokenType::If:
    statementIf();
    return;
  case TokenType::While:
    statementWhile();
    return;
  case TokenType::Return:
    nextToken();
    builder.CreateRet(expression());
    builder.SetInsertPoint(llvm::BasicBlock::Create(context, "dummy"));
    return;
  case TokenType::Write:
    nextToken();
    writeFunc("%d", expression());
    break;
  case TokenType::Writeln:
    nextToken();
    writeFunc("\n");
    break;
  case TokenType::QFunc:
    nextToken();
    builder.CreateCall(quantumFunc);
    // builder.CreateCall(quantumFunc, std::vector<llvm::Value *>(1, expression()));
    break;
  default:;
  }
}

void Frontend::statementAssign() {
  const auto &info = ident_table.find(cur_token.ident);
  llvm::Value *assignee;
  if (info.type == IdType::Var) {
    assignee = info.val;
  } else if (info.type == IdType::Qint) {
    assignee = info.val;
  } else {
    error("variable is expected but it is not variable");
  }

  nextToken();
  takeToken(TokenType::Assign);
  if (cur_token.type == TokenType::Ident) {
    const auto &infotarget = ident_table.find(cur_token.ident);

    if (info.type == IdType::Qint && infotarget.type == IdType::Qint) {
      telepcall(infotarget.qreg, info.qreg);
      nextToken();
      return;
    }
  } else if (info.type == IdType::Qint && cur_token.type == TokenType::Integer) {
    telepcall(info.qreg, QuantumRegister::q0);
    nextToken();
    return;
  }

  builder.CreateStore(expression(), assignee);
  return;
}

void Frontend::statementIf() {
  takeToken(TokenType::If);
  auto *cond = condition();

  auto *then_block = llvm::BasicBlock::Create(context, "if.then", curFunc);
  auto *merge_block = llvm::BasicBlock::Create(context, "if.merge");

  builder.CreateCondBr(cond, then_block, merge_block);

  builder.SetInsertPoint(then_block);
  statement();
  builder.CreateBr(merge_block);
  then_block = builder.GetInsertBlock();

  curFunc->getBasicBlockList().push_back(merge_block);
  builder.SetInsertPoint(merge_block);
}

void Frontend::statementWhile() {
  takeToken(TokenType::While);

  auto *cond_block = llvm::BasicBlock::Create(context, "while.cond", curFunc);
  auto *body_block = llvm::BasicBlock::Create(context, "while.body");
  auto *merge_block = llvm::BasicBlock::Create(context, "while.merge");

  builder.CreateBr(cond_block);

  {
    builder.SetInsertPoint(cond_block);
    auto *cond = condition();
    builder.CreateCondBr(cond, body_block, merge_block);
  }

  {
    curFunc->getBasicBlockList().push_back(body_block);
    builder.SetInsertPoint(body_block);
    statement();
    builder.CreateBr(cond_block);
  }

  curFunc->getBasicBlockList().push_back(merge_block);
  builder.SetInsertPoint(merge_block);
}

llvm::CmpInst::Predicate token_to_inst(TokenType type) {
  switch (type) {
  case TokenType::Equal:
    return llvm::CmpInst::Predicate::ICMP_EQ;
  case TokenType::NotEqual:
    return llvm::CmpInst::Predicate::ICMP_NE;
  case TokenType::Less:
    return llvm::CmpInst::Predicate::ICMP_SLT;
  case TokenType::LessEqual:
    return llvm::CmpInst::Predicate::ICMP_SLE;
  case TokenType::Greater:
    return llvm::CmpInst::Predicate::ICMP_SGT;
  case TokenType::GreaterEqual:
    return llvm::CmpInst::Predicate::ICMP_SGE;
  default:
    throw "not support at token to inst";
  }
}

llvm::Value *Frontend::condition() {
  if (cur_token.type == TokenType::Odd) {
    nextToken();
    auto *lhs = builder.CreateSRem(expression(), builder.getInt64(2));
    return builder.CreateICmpEQ(lhs, builder.getInt64(0));
  } else {
    auto *lhs = expression();
    llvm::CmpInst::Predicate inst = token_to_inst(cur_token.type);
    nextToken();
    auto *rhs = expression();
    return builder.CreateICmp(inst, lhs, rhs);
  }
}

llvm::Value *Frontend::expression() {
  TokenType sign = cur_token.type;
  if (cur_token.type == TokenType::Plus || cur_token.type == TokenType::Minus) {
    nextToken();
  }

  llvm::Value *ret = term();
  if (sign == TokenType::Minus) {
    ret = builder.CreateNeg(ret);
  }
  
  while (true) {
    if (cur_token.type == TokenType::Plus) {
      nextToken();
      ret = builder.CreateAdd(ret, term());
    } else if (cur_token.type == TokenType::Minus) {
      nextToken();
      ret = builder.CreateSub(ret, term());
    } else {
      break;
    }
  }
  return ret;
}

llvm::Value *Frontend::term() {
  llvm::Value *ret = factor();
  while (true) {
    if (cur_token.type == TokenType::Mul) {
      nextToken();
      ret = builder.CreateMul(ret, factor());
    } else if (cur_token.type == TokenType::Div) {
      nextToken();
      ret = builder.CreateSDiv(ret, factor());
    } else {
      break;
    }
  }
  return ret;
}

llvm::Value *Frontend::factor() {
  llvm::Value *ret;
  if (cur_token.type == TokenType::Ident) {
    ret = factorIdent();
  } else if (cur_token.type == TokenType::Qint) {
    ret = factorIdent();
  } else if (cur_token.type == TokenType::Integer) {
    ret = builder.getInt64(cur_token.integer);
    nextToken();
  } else if (cur_token.type == TokenType::ParenL) {
    nextToken();
    ret = expression();
    takeToken(TokenType::ParenR);
  } else {
    lexer.print_head();
    error("expect factor but not");
  }
  return ret;
}

llvm::Value *Frontend::factorIdent() {
  auto &val = ident_table.find(cur_token.ident);
  takeToken(TokenType::Ident);

  switch (val.type) {
  case IdType::Const:
    return val.val;
  case IdType::Var:
    return builder.CreateLoad(val.val);
  case IdType::Qint:
    return qmeascall(val.qreg);
  case IdType::Param:
    error("Param ident can not be factor");
  case IdType::Function:
    takeToken(TokenType::ParenL);
    std::vector<llvm::Value *> args;
    while (cur_token.type != TokenType::ParenR) {
      args.push_back(expression());
      if (cur_token.type == TokenType::Colon) {
        nextToken();
        // continue;
      } else {
        break;
      }
    }
    takeToken(TokenType::ParenR);
    if (args.size() != val.func->arg_size()) {
      error("argument number is wrong");
    }
    return builder.CreateCall(val.func, args);
  }
}

