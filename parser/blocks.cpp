#include <llvm/IR/Instruction.h>
#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"

void createBlocks(const llvm::Module* module, Program& program) {
    for (const llvm::Function& func : module->functions()) {
        auto* function = program.getFunction(&func);
        for (const auto& block : func) {
            function->createBlockIfNotExist(&block);
        }
    }
}
