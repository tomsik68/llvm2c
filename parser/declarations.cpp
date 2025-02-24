#include <llvm/IR/Instruction.h>

#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"

#include <iostream>

static void declareFunc(const llvm::Function* func, Program& program) {
    auto decl = std::make_unique<Func>(func, &program, true);
    const llvm::Value* lastValue = nullptr;
    for (const llvm::Value& arg : func->args()) {
        lastValue = &arg;

        auto argVal = std::make_unique<Value>(decl->getVarName(), program.getType(arg.getType()));

        decl->createExpr(&arg, std::move(argVal));
    }

    decl->setVarArg(func->isVarArg());

    program.addDeclaration(func, std::move(decl));
}

void findDeclaredFunctions(const llvm::Module *module, Program& program) {
    for(const llvm::Function& func : module->functions()) {
        if (func.hasName()) {
            if (func.isDeclaration() || llvm::Function::isInternalLinkage(func.getLinkage())) {
                if (func.getName().str().substr(0, 8) != "llvm.dbg") {
                    if (!program.isFunctionDeclared(&func)) {
                        declareFunc(&func, program);
                    }
                }
            }

            if (!func.isDeclaration()) {
                if (!program.isFunctionDeclared(&func)) {
                    declareFunc(&func, program);
                }
            }
        }
    }
}
