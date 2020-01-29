#if 1
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include <vector>
#include <stack>
using namespace llvm;

namespace {
  struct quantumOptimizerPass : public FunctionPass {
    static char ID;
    quantumOptimizerPass() : FunctionPass(ID) {}
    std::stack<Instruction *> dupErasekList;
    std::vector<std::string> dupEraseQuantumAsm = {"qooz.k", "qoox.k"};

    virtual bool runOnFunction(Function &F) {
      for (llvm::BasicBlock &BB : F) {
        for (llvm::Instruction &II: BB) {
          std::string str;
          llvm::raw_string_ostream rso(str);
          II.print(rso);

          // ....？
          std::size_t found = str.find("qtelep.k");
          if (std::string::npos != found) {
            dupErasekList.push(&II);
          }

          for (Use &U:II.operands()) {
            Value *v = U.get();
            
            if (dyn_cast<Instruction>(v)) {
              errs() << "\"" << *dyn_cast<Instruction>(v) << "\"" << " -> " << "\"" << II << "\"" << ";\n";
            }
            if (v->getName() != "") {
              errs() << "\"" << v->getName() << "\"" << " -> " << "\"" << II << "\"" << ";\n";
              errs() << "\"" << v->getName() << "\"" << "\n";
            }
          }
        }
      }

/*
      while (!dupErasekList.empty()) {
        Instruction *II = dupErasekList.top();
        II->eraseFromParent();
        dupErasekList.pop();
      }
      */
      return false;
    }
  };
}

char quantumOptimizerPass::ID = 0;
static void registerquantumOptimizerPass(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
  PM.add(new quantumOptimizerPass());
}
static RegisterStandardPasses
  RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                 registerquantumOptimizerPass);
#endif
