#include "Program.h"

#include <llvm/IR/LLVMContext.h>
#include "llvm/Support/raw_ostream.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/IR/Constants.h"

#include "Type.h"

#include <fstream>
#include <exception>
#include <algorithm>

Program::Program(const std::string &file) {
    error = llvm::SMDiagnostic();
    module = llvm::parseIRFile(file, error, context);
    if(!module) {
        throw std::invalid_argument("Error loading module!");
    }
    structVarCount = 0;
    gvarCount = 0;

    hasVarArg = false;

    parseProgram();
}

void Program::parseProgram() {
    parseGlobalVars();
    parseStructs();
    parseFunctions();
}

void Program::parseStructs() {
    for (llvm::StructType* structType : module->getIdentifiedStructTypes()) {
        std::string name = "";
        if (structType->hasName()) {
            name = structType->getName().str();
            name.erase(0, 7);
        }

        if (name.compare("__va_list_tag") == 0) {
            hasVarArg = true;
            auto structExpr = std::make_unique<Struct>(name);
            structExpr->addItem(std::make_unique<IntType>(true), "gp_offset");
            structExpr->addItem(std::make_unique<IntType>(true), "fp_offset");
            structExpr->addItem(std::make_unique<PointerType>(std::make_unique<VoidType>()), "overflow_arg_area");
            structExpr->addItem(std::make_unique<PointerType>(std::make_unique<VoidType>()), "reg_save_area");
            structs.push_back(std::move(structExpr));
            continue;
        }

        auto structExpr = std::make_unique<Struct>(name);

        for (llvm::Type* type : structType->elements()) {
            structExpr->addItem(Type::getType(type), getStructVarName());
        }

        structs.push_back(std::move(structExpr));
    }
}

void Program::parseFunctions() {
    for(const llvm::Function& func : module->functions()) {
        if (func.hasName()) {
            if (func.isDeclaration()) {
                if (func.getName().str().substr(0, 8) != "llvm.dbg") {
                    declarations.push_back(std::make_unique<Func>(&func, this, true));
                }
            } else {
                functions.push_back(std::make_unique<Func>(&func, this, false));
            }
        } 
    }
}

void Program::parseGlobalVars() {
    for (const llvm::GlobalVariable& gvar : module->globals()) {
        std::string gvarName;
        if (gvar.hasName()) {
            if (gvar.hasPrivateLinkage()) {
                gvarName = "ConstGlobalVar";
            }
            std::string replacedName = gvar.getName().str();
            std::replace(replacedName.begin(), replacedName.end(), '.', '_');
            gvarName = "&" + gvarName + replacedName;
        } else {
            gvarName = getGvarName();
        }

        std::string value = "";
        if (gvar.hasInitializer()) {
            value = getValue(gvar.getInitializer());
        }

        llvm::PointerType* PI = llvm::cast<llvm::PointerType>(gvar.getType());
        globalVars[&gvar] = std::make_unique<GlobalValue>(gvarName, value, std::move(Type::getType(PI->getElementType())));
    }
}

std::string Program::getStructVarName() {
    std::string varName = "structVar";
    varName += std::to_string(structVarCount);
    structVarCount++;

    return varName;
}

std::string Program::getGvarName() {
    std::string varName = "&gvar";
    varName += std::to_string(gvarCount);
    structVarCount++;

    return varName;
}

std::string Program::getValue(const llvm::Constant* val) const {
    if (llvm::PointerType* PT = llvm::dyn_cast<llvm::PointerType>(val->getType())) {
        return "&" + val->getName().str();
    }

    if (llvm::ConstantInt* CI = llvm::dyn_cast<llvm::ConstantInt>(val)) {
        return std::to_string(CI->getSExtValue());
    }
    if (llvm::ConstantFP* CFP = llvm::dyn_cast<llvm::ConstantFP>(val)) {
        return std::to_string(CFP->getValueAPF().convertToFloat());
    }
    if (llvm::ConstantDataArray* CDA = llvm::dyn_cast<llvm::ConstantDataArray>(val)) {
        std::string value = "{";
        bool first = true;

        for (unsigned i = 0; i < CDA->getNumElements(); i++) {
            if (!first) {
                value += ", ";
            }
            first = false;

            value += getValue(CDA->getElementAsConstant(i));
        }

        return value + "}";
    }

    return "";
}

void Program::unsetAllInit() {
    for (const llvm::GlobalVariable& gvar : module->globals()) {
        globalVars[&gvar]->init = false;
    }

    for (auto& strct : structs) {
        strct->isPrinted = false;
    }
}

void Program::print() {
    unsetAllInit();

    if (hasVarArg) {
        llvm::outs() << "#include <stdarg.h>\n\n";
    }

    for (const auto& func : declarations) {
        func->print();
    }

    for (auto& strct : structs) {
        if (!strct->isPrinted) {
            printStruct(strct.get());
        }
    }

    for (const auto& global : module->globals()) {
        globalVars[&global]->print();
        globalVars[&global]->init = true;
        llvm::outs() << "\n";
    }
    llvm::outs() << "\n";

    for (const auto& func : functions) {
        func->print();
    }

    llvm::outs().flush();
}

void Program::printStruct(Struct* strct) {
    for (auto& item : strct->items) {
        Type* type = item.first.get();
        if (auto AT = dynamic_cast<ArrayType*>(item.first.get())) {
            type = AT->type.get();
        }

        if (auto ST = dynamic_cast<StructType*>(type)) {
            for (auto& s : structs) {
                if (s->name == ST->name) {
                    printStruct(s.get());
                    llvm::outs() << "\n";
                }
            }
        }
    }
    strct->print();
    strct->isPrinted = true;
    llvm::outs() << "\n";
}

void Program::saveStruct(Struct* strct, std::ofstream& file) {
    for (auto& item : strct->items) {
        Type* type = item.first.get();
        if (auto AT = dynamic_cast<ArrayType*>(item.first.get())) {
            type = AT->type.get();
        }

        if (auto ST = dynamic_cast<StructType*>(type)) {
            for (auto& s : structs) {
                if (s->name == ST->name) {
                    saveStruct(s.get(), file);
                    file << "\n";
                }
            }
        }
    }
    file << strct->toString();
    strct->isPrinted = true;
    file << "\n";
}

void Program::saveFile(const std::string& fileName) {
    unsetAllInit();

    std::ofstream file;
    file.open(fileName);

    if (hasVarArg) {
        file << "#include <stdarg.h>\n\n";
    }

    for (const auto& func : declarations) {
        func->saveFile(file);
    }

    for (auto& strct : structs) {
        if (!strct->isPrinted) {
            saveStruct(strct.get(), file);
        }
    }

    for (const auto& global : module->globals()) {
        file << globalVars[&global]->toString();
        globalVars[&global]->init = true;
        file << "\n";
    }
    file << "\n";

    for (const auto& func : functions) {
        func->saveFile(file);
    }

    file.close();
}

Struct* Program::getStruct(const std::string& name) const {
    for (const auto& strct : structs) {
        if (strct->name == name) {
            return strct.get();
        }
    }

    return nullptr;
}

GlobalValue* Program::getGlobalVar(llvm::Value* val) const {
    llvm::GlobalVariable* GV = llvm::cast<llvm::GlobalVariable>(val);
    return globalVars[GV].get();
}

void Program::addDeclaration(llvm::Function* func) {
    declarations.push_back(std::make_unique<Func>(func, this, true));
}
