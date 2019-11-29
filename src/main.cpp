
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/ValueSymbolTable.h>
#include <llvm/IR/InlineAsm.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>
#include <string>

#include "llvm_frontend.hpp"

using namespace qlang;

int main(int argc, char **argv) {

  qlang::Frontend frontend(argv[1]);
  frontend.compile();

  llvm::legacy::PassManager pm;

  // generate bitcode
  // TODO: divide main source code and quantum code.
  std::error_code error_info;
  llvm::raw_fd_ostream raw_stream("out.ll", error_info,
                                  llvm::sys::fs::OpenFlags::F_None);
  pm.add(llvm::createPrintModulePass(raw_stream));
  pm.run(*frontend.getModule());
  raw_stream.close();

  return 0;
}