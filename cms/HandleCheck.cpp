//===--- HandleCheck.cpp - clang-tidy--------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "HandleCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include <iostream>

using namespace clang::ast_matchers;
namespace clang {
namespace tidy {
namespace cms {

void HandleCheck::registerMatchers(MatchFinder *Finder) {
  // FIXME: Add matchers.
  Finder->addMatcher(cxxMemberCallExpr().bind("member"), this);
}

void HandleCheck::check(const MatchFinder::MatchResult &Result) {
  // FIXME: Add callback implementation.
  std::string edmgettoken = "edm::EDGetTokenT";
  std::string getbytoken = "getByToken";
  std::string edmhandle = "edm::Handle";
  std::string gethandle = "getHandle";
  const auto *MatchedCallExpr = Result.Nodes.getNodeAs<CXXMemberCallExpr>("member");
  if (MatchedCallExpr){
    auto MatchedDecl = MatchedCallExpr->getMethodDecl();
    auto MatchedName = MatchedDecl->getNameAsString();
    auto callstart = MatchedCallExpr->getLocStart();
    auto callrange= MatchedCallExpr->getSourceRange();
    if (MatchedName.compare(getbytoken) == 0) {
      for (auto I: MatchedCallExpr->arguments()) {
         auto argtype = I->getType();
         auto tname = argtype.getAsString();
         llvm::errs() << "CXXMemberCallExpr ";
         MatchedCallExpr->dumpColor();
         llvm::errs()<<"\n";
         llvm::errs() << "CXXMemberDecl ";
         MatchedDecl->dumpColor();
         llvm::errs() <<"\n";
         llvm::errs() << "ArgumentExpr ";
         I->dump();
         llvm::errs()<<"\n";
         llvm::errs() << "ArgumentType ";
         argtype->dump();
         llvm::errs()<<"\n";
         if ( tname.compare(0,edmgettoken.size(),edmgettoken) == 0 ) {
             llvm::errs() <<edmgettoken<<" type found\n";
         }    
         if ( tname.compare(0,edmhandle.size(),edmhandle) == 0 ) { 
             auto D = llvm::dyn_cast<DeclRefExpr>(I)->getDecl();
             llvm::errs() << edmhandle <<" type found\n";
             auto declend = D->getEndLoc();
             diag(declend, "use function iEvent." + gethandle + " to initialize " + edmhandle +"<T>", DiagnosticIDs::Warning)
              << FixItHint::CreateInsertion(declend, " = iEvent.getHandle()");
         }
      }
      diag(callstart, "function " + MatchedName + " is deprecated", DiagnosticIDs::Warning)
        << FixItHint::CreateRemoval(callrange);
    }
  }
}

} // namespace cms
} // namespace tidy
} // namespace clang
