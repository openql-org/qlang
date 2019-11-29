
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/ValueSymbolTable.h>
#include <llvm/IR/InlineAsm.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/InitLLVM.h>

#include <string>

#include "llvm_frontend.hpp"
#include "option_parser.h"

using namespace qlang;

static void help(int exit_code = 1)
{
  fprintf(stderr, "qlang compiler\n\n");
  fprintf(stderr, "usage: qlang [options] <target source code>\n");
  fprintf(stderr, "options:\n");
  fprintf(stderr, "  -O<n>                 optimizer leve\n");
  exit(exit_code);
}

static void suggest_help() {
  fprintf(stderr, "Try 'qlang --help' for more information.\n");
  exit(1);
}

int main(int argc, char **argv) {
  llvm::InitLLVM X(argc, argv);
  int optimize = 0;

  option_parser_t parser;
  parser.help(&suggest_help);
  parser.option('h', "help", 0, [&](const char* s){help(0);});
  parser.option('O', 0, 1, [&](const char* s){optimize = atoi(s);});
  auto argv1 = parser.parse(argv);
  std::vector<std::string> htif_args(argv1, (const char*const*)argv + argc);

  if (!*argv1)
    help();

  qlang::Frontend frontend(htif_args[0]);
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