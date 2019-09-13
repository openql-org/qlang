#include "qlang.h"

int main() {
  llvm::LLVMContext context;
  llvm::Module *module = new llvm::Module("top", context);
  llvm::IRBuilder<> builder(context);

  auto *funcType = llvm::FunctionType::get(builder.getInt64Ty(), false);
  auto *mainFunc = llvm::Function::Create(
      funcType, llvm::Function::ExternalLinkage, "main", module);
  auto *entrypoint = llvm::BasicBlock::Create(context, "entrypoint", mainFunc);

  builder.SetInsertPoint(entrypoint);

  builder.CreateRet(builder.getInt64(0));

  std::error_code error_info;
  llvm::raw_fd_ostream raw_stream("out.ll", error_info,
                                  llvm::sys::fs::OpenFlags::F_None);
  module->print(raw_stream, nullptr);

  // module->print(llvm::outs(), nullptr); // print LLVM IR to stdout
  // llvm::WriteBitcodeToFile(module, raw_stream); // write LLVM Bitcode to file
  return 0;
}
