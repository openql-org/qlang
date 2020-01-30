#if 1
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
    std::multimap<std::string, Instruction *> dupErasekMap;
    std::vector<std::string> dupEraseQuantumAsm = {"qooz.k"};

    virtual bool runOnFunction(Function &F) {
      for (llvm::BasicBlock &BB : F) {
        for (llvm::Instruction &II: BB) {
          std::string str;
          llvm::raw_string_ostream rso(str);
          II.print(rso);

          // remove comment
          int npos = str.find("#");
          if (npos > 0) str.erase(npos - 1);

          // just simple duplicate gate only now.
          for (std::string &v : dupEraseQuantumAsm) {
            std::size_t found = str.find(v);
            if (std::string::npos != found) {
              if (dupErasekMap.count(str) == 1) {
                auto itr = dupErasekMap.find(str);
                dupErasekList.push(itr->second);
                dupErasekList.push(&II);
                dupErasekMap.erase(str);
              } else 
                dupErasekMap.insert(std::make_pair(str, &II));
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
#endif
