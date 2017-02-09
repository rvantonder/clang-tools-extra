//===---- tools/extra/ToolTemplate.cpp - Template for refactoring tool ----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file implements an empty refactoring tool using the clang tooling.
//  The goal is to lower the "barrier to entry" for writing refactoring tools.
//
//  Usage:
//  tool-template <cmake-output-dir> <file1> <file2> ...
//
//  Where <cmake-output-dir> is a CMake build directory in which a file named
//  compile_commands.json exists (enable -DCMAKE_EXPORT_COMPILE_COMMANDS in
//  CMake to get this output).
//
//  <file1> ... specify the paths of files in the CMake source tree. This path
//  is looked up in the compile command database. If the path of a file is
//  absolute, it needs to point into CMake's source tree. If the path is
//  relative, the current working directory needs to be in the CMake source
//  tree and the file must be in a subdirectory of the current working
//  directory. "./" prefixes in the relative files will be automatically
//  removed, but the rest of a relative path must be a suffix of a path in
//  the compile command line database.
//
//  For example, to use tool-template on all files in a subtree of the
//  source tree, use:
//
//    /path/in/subtree $ find . -name '*.cpp'|
//        xargs tool-template /path/to/build
//
//===----------------------------------------------------------------------===//

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Lex/Lexer.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Refactoring.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Signals.h"
#include "clang/Basic/DiagnosticOptions.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;
using namespace llvm;

StatementMatcher LoopMatcher =
  forStmt(hasLoopInit(declStmt(hasSingleDecl(varDecl(
    hasInitializer(integerLiteral(equals(0)))))))).bind("forLoop");

namespace {
class ToolTemplateCallback : public MatchFinder::MatchCallback {
public:
  ToolTemplateCallback(std::map<std::string, Replacements> *Replace)
      : Replace(Replace) {}

  void run(const MatchFinder::MatchResult &Result) override {
    // TODO: This routine will get called for each thing that the matchers
    // find.
    // At this point, you can examine the match, and do whatever you want,
    // including replacing the matched text with other text
    if (const ForStmt *FS = Result.Nodes.getNodeAs<clang::ForStmt>("forLoop")) {
      FS->dump(); // dumps the ENTIRE for statement
      Replacement Rep(*(Result.SourceManager), FS->getLocStart(), 0,
                     "// for comment\n");

      Replacements Reps(Rep);
      std::string s1 ("/tmp/simple-loops.cc");
      Replace->insert(std::pair<std::string,Replacements>(s1,Reps));
      // Replace->insert(add(Rep); // add replacement
    }

    // (void)Replace; // This to prevent an "unused member variable" warning;
  }

private:
  std::map<std::string, Replacements> *Replace;
};
} // end anonymous namespace

// Set up the command line options
static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);
static cl::OptionCategory ToolTemplateCategory("tool-template options");

int main(int argc, const char **argv) {
  llvm::sys::PrintStackTraceOnErrorSignal(argv[0]);
  CommonOptionsParser OptionsParser(argc, argv, ToolTemplateCategory);
  RefactoringTool Tool(OptionsParser.getCompilations(),
                       OptionsParser.getSourcePathList());
  ast_matchers::MatchFinder Finder;
  ToolTemplateCallback Callback(&Tool.getReplacements());

  // TODO: Put your matchers here.
  Finder.addMatcher(LoopMatcher, &Callback); // Use Find.addMatcher(...) to
                                            // define the patterns in the AST
                                            // that you
  // want to match against. You are not limited to just one matcher!

  int ret = Tool.run(newFrontendActionFactory(&Finder).get());

  LangOptions DefaultLangOptions;
  IntrusiveRefCntPtr<DiagnosticOptions> DiagOpts = new DiagnosticOptions();
  TextDiagnosticPrinter DiagnosticPrinter(errs(), &*DiagOpts);
  DiagnosticsEngine Diagnostics(
        IntrusiveRefCntPtr<DiagnosticIDs>(new DiagnosticIDs()), &*DiagOpts,
        &DiagnosticPrinter, false);
  auto &FileMgr = Tool.getFiles();
  SourceManager Sources(Diagnostics, FileMgr);
  Rewriter Rewrite(Sources, DefaultLangOptions);

  auto Files = OptionsParser.getSourcePathList();

  Tool.applyAllReplacements(Rewrite);

  for (const auto &File : Files) {
    const auto *Entry = FileMgr.getFile(File);
    const auto ID = Sources.getOrCreateFileID(Entry, SrcMgr::C_User);
    Rewrite.getEditBuffer(ID).write(outs());
  }

  return ret;
}
