#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include <vector>
#include <map>
#include <stack>
using namespace llvm;

namespace {
  struct quantumDupOptimizerPass : public FunctionPass {
    static char ID;
    quantumDupOptimizerPass() : FunctionPass(ID) {}
    std::stack<Instruction *> dupErasekList;
    std::map<std::string, Instruction *> dupErasekMap;
    std::vector<std::string> dupEraseQuantumAsm = {"qtelep.k", "qoox.k"};

    virtual bool runOnFunction(Function &F) {
      for (llvm::BasicBlock &BB : F) {
        for (llvm::Instruction &II: BB) {
          std::string str;
          llvm::raw_string_ostream rso(str);
          II.print(rso);

          // more better?
          for (std::string &v : dupEraseQuantumAsm) {
            std::size_t found = str.find(v);
            if (std::string::npos != found) {
              if (dupErasekMap.count(str) == 0)
                dupErasekMap[str] = &II;
              else
                dupErasekList.push(&II);
            }
          }
        }
      }

      while (!dupErasekList.empty()) {
        Instruction *II = dupErasekList.top();
        // errs() << *II << "\n";
        II->eraseFromParent();
        dupErasekList.pop();
      }
      return false;
    }
  };
}

char quantumDupOptimizerPass::ID = 0;
static void registerquantumDupOptimizerPass(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
  PM.add(new quantumDupOptimizerPass());
}
static RegisterStandardPasses
  RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                 registerquantumDupOptimizerPass);