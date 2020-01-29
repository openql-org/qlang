#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
using namespace llvm;

namespace {
  struct quantumOptimizerPass : public FunctionPass {
    static char ID;
    quantumOptimizerPass() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
      for (llvm::BasicBlock &BB : F) {
        for (llvm::Instruction &II: BB) {
        }
      }
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
