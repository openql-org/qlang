
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/ValueSymbolTable.h>
#include <llvm/IR/InlineAsm.h>
#include <llvm/ADT/Triple.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/CodeGen/CommandFlags.inc>

#include <llvm/Support/InitLLVM.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Analysis/TargetLibraryInfo.h>
#include <llvm/Analysis/TargetTransformInfo.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>
#include <llvm/ADT/DenseMap.h>
#include <llvm/IR/PassManager.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/Transforms/IPO/AlwaysInliner.h>
#include <llvm/Transforms/IPO/Inliner.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Transforms/Coroutines.h>
#include <llvm/Transforms/IPO/AlwaysInliner.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>
#include <llvm/Transforms/Utils/Cloning.h>
#include <llvm/InitializePasses.h>
#include <llvm/LinkAllIR.h>
#include <llvm/LinkAllPasses.h>


#include <string>

#include "llvm_frontend.hpp"
#include "option_parser.h"

using namespace qlang;


static cl::opt<bool>
OptLevelO0("O0",
  cl::desc("Optimization level 0. Similar to clang -O0"));

static cl::opt<bool>
OptLevelO1("O1",
           cl::desc("Optimization level 1. Similar to clang -O1"));

static cl::opt<bool>
OptLevelO2("O2",
           cl::desc("Optimization level 2. Similar to clang -O2"));

static cl::opt<bool>
OptLevelO3("O3",
           cl::desc("Optimization level 3. Similar to clang -O3"));

static cl::opt<unsigned>
CodeGenOptLevel("codegen-opt-level",
                cl::desc("Override optimization level for codegen hooks"));

static CodeGenOpt::Level GetCodeGenOptLevel() {
  if (CodeGenOptLevel.getNumOccurrences())
    return static_cast<llvm::CodeGenOpt::Level>(unsigned(CodeGenOptLevel));
  if (OptLevelO1)
    return llvm::CodeGenOpt::Less;
  if (OptLevelO2)
    return llvm::CodeGenOpt::Default;
  if (OptLevelO3)
    return llvm::CodeGenOpt::Aggressive;
  return llvm::CodeGenOpt::None;
}

// Returns the TargetMachine instance or zero if no triple is provided.
static TargetMachine* GetTargetMachine(Triple TheTriple, StringRef CPUStr,
                                       StringRef FeaturesStr,
                                       const TargetOptions &Options) {
  std::string Error;
  const Target *TheTarget = TargetRegistry::lookupTarget(MArch, TheTriple,
                                                         Error);
  // Some modules don't specify a triple, and this is okay.
  if (!TheTarget) {
    return nullptr;
  }

  return TheTarget->createTargetMachine(TheTriple.getTriple(), CPUStr,
                                        FeaturesStr, Options, getRelocModel(),
                                        getCodeModel(), GetCodeGenOptLevel());
}

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

  llvm::LLVMContext Context;
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmPrinters();
  llvm::InitializeAllAsmParsers();

  llvm::PassRegistry &Registry = *llvm::PassRegistry::getPassRegistry();
  llvm::initializeCore(Registry);
  llvm::initializeCoroutines(Registry);
  llvm::initializeScalarOpts(Registry);
  llvm::initializeObjCARCOpts(Registry);
  llvm::initializeVectorization(Registry);
  llvm::initializeIPO(Registry);
  llvm::initializeAnalysis(Registry);
  llvm::initializeTransformUtils(Registry);
  llvm::initializeInstCombine(Registry);
  llvm::initializeAggressiveInstCombine(Registry);
  llvm::initializeInstrumentation(Registry);
  llvm::initializeTarget(Registry);
  llvm::initializeExpandMemCmpPassPass(Registry);
  llvm::initializeScalarizeMaskedMemIntrinPass(Registry);
  llvm::initializeCodeGenPreparePass(Registry);
  llvm::initializeAtomicExpandPass(Registry);
  llvm::initializeRewriteSymbolsLegacyPassPass(Registry);
  llvm::initializeWinEHPreparePass(Registry);
  llvm::initializeDwarfEHPreparePass(Registry);
  llvm::initializeSafeStackLegacyPassPass(Registry);
  llvm::initializeSjLjEHPreparePass(Registry);
  llvm::initializeStackProtectorPass(Registry);
  llvm::initializePreISelIntrinsicLoweringLegacyPassPass(Registry);
  llvm::initializeGlobalMergePass(Registry);
  llvm::initializeIndirectBrExpandPassPass(Registry);
  llvm::initializeInterleavedLoadCombinePass(Registry);
  llvm::initializeInterleavedAccessPass(Registry);
  llvm::initializeEntryExitInstrumenterPass(Registry);
  llvm::initializePostInlineEntryExitInstrumenterPass(Registry);
  llvm::initializeUnreachableBlockElimLegacyPassPass(Registry);
  llvm::initializeExpandReductionsPass(Registry);
  llvm::initializeWasmEHPreparePass(Registry);
  llvm::initializeWriteBitcodePassPass(Registry);
  llvm::initializeHardwareLoopsPass(Registry);

  // compile source code.
  qlang::Frontend frontend(htif_args[0]);
  frontend.compile();
  llvm::legacy::PassManager pm;

  // optimize legacy code
  if (optimize > 0) {
    llvm::Triple ModuleTriple(frontend.getModule()->getTargetTriple());
    std::string CPUStr, FeaturesStr;
    TargetMachine *Machine = nullptr;
    const TargetOptions Options = InitTargetOptionsFromCodeGenFlags();

    if (ModuleTriple.getArch()) {
      CPUStr = getCPUStr();
      FeaturesStr = getFeaturesStr();
      Machine = GetTargetMachine(ModuleTriple, CPUStr, FeaturesStr, Options);
    }
    std::unique_ptr<TargetMachine> TM(Machine);
    setFunctionAttributes(CPUStr, FeaturesStr,*frontend.getModule());
    llvm::TargetLibraryInfoImpl TLII(ModuleTriple);

    pm.add(createTargetTransformInfoWrapperPass(TM ? TM->getTargetIRAnalysis() : TargetIRAnalysis()));
    std::unique_ptr<legacy::FunctionPassManager> FPasses;
    FPasses.reset(new legacy::FunctionPassManager(frontend.getModule()));
    FPasses->add(createTargetTransformInfoWrapperPass(TM ? TM->getTargetIRAnalysis() : TargetIRAnalysis()));

    llvm::PassManagerBuilder Builder;
    Builder.OptLevel = optimize;
    Builder.SizeLevel = 0;
    Builder.Inliner = llvm::createFunctionInliningPass(Builder.OptLevel, Builder.SizeLevel, false);
    Builder.populateFunctionPassManager(*FPasses);
    Builder.populateModulePassManager(pm);
  }

  // generate bitcode
  std::error_code error_info;
  llvm::raw_fd_ostream raw_stream("out.ll", error_info,
                                  llvm::sys::fs::OpenFlags::F_None);
  pm.add(llvm::createPrintModulePass(raw_stream));
  pm.run(*frontend.getModule());
  raw_stream.close();

  return 0;
}