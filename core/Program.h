#pragma once

#include <vector>
#include <set>

#include <llvm/Support/SourceMgr.h>
#include <llvm/IR/Module.h>
#include "llvm/ADT/DenseMap.h"

#include "Func.h"
#include "../expr/Expr.h"
#include "../type/TypeHandler.h"

class ProgramParser;

/**
 * @brief The Program class represents the whole parsed LLVM program.
 */
class Program {
friend class TypeHandler;
friend class Func;
friend class ProgramParser;
public:
    //std::unique_ptr<llvm::Module> module;

    TypeHandler typeHandler;

    //expressions
    llvm::DenseMap<const llvm::Function*, std::unique_ptr<Func>> functions; //map containing function definitions
    llvm::DenseMap<const llvm::Function*, std::unique_ptr<Func>> declarations; //map containing function declarations
    std::vector<std::unique_ptr<Struct>> structs; // vector of parsed structs
    std::vector<std::unique_ptr<GlobalValue>> globalVars; // vector of parsed global variables
    llvm::DenseMap<const llvm::GlobalVariable*, std::unique_ptr<RefExpr>> globalRefs; //map containing references to global variables
    llvm::DenseMap<const llvm::StructType*, std::unique_ptr<Struct>> unnamedStructs; // map containing unnamed structs

    //set containing names of global variables that are in "var[0-9]+" format, used in creating variable names in functions
    std::set<std::string> globalVarNames;

    //variables used for creating names for structs and anonymous structs
    unsigned structVarCount = 0;
    unsigned anonStructCount = 0;

    /**
     * @brief getAnonStructName Creates new name for anonymous struct.
     * @return New name for anonymous struct
     */
    std::string getAnonStructName();

    /**
     * @brief getInitValue Return string containing value used for global variable initialization.
     * @param val llvm Constant used for initialization
     * @return Init value
     */
    std::string getInitValue(const llvm::Constant* val);

    /**
     * @brief unsetAllInit Resets the init flag for every global variable.
     * Used for repeated calling of print and saveFile.
     */
    void unsetAllInit();

    /**
     * @brief saveStruct Saves parsed Struct into the file. If Struct contains other Struct, then the other is saved first.
     * @param strct Struct for saving
     * @param file Opened file for saving the struct.
     */
    void saveStruct(Struct* strct, std::ofstream& file);

    /**
     * @brief printStruct Prints parsed Struct. If Struct contains other Struct, then the other is printed first.
     * @param strct Struct for printing
     */
    void printStruct(Struct* strct);

    /**
     * @brief getIncludeString Returns string containing all includes program uses.
     * @return String containing includes;
     */
    std::string getIncludeString() const;

    /**
     * @brief output Outputs the translated program to given stream.
     * @param stream Stream for output
     */
    void output(std::ostream& stream);

    /**
     * @brief outputStruct Outputs parsed Struct to given stream. If Struct contains other Struct, then the other is output first.
     * @param strct Struct for output
     * @param stream Stream for output
     */
    void outputStruct(Struct* strct, std::ostream& stream);

    void createNewUnnamedStruct(const llvm::StructType *strct);

public:
    bool stackIgnored = false; //instruction stacksave was ignored

    bool hasVarArg = false; //program uses "stdarg.h"
    bool hasStdLib = false; //program uses "stdlib.h"
    bool hasString = false; //program uses "string.h"
    bool hasStdio = false; //program uses "stdio.h"
    bool hasPthread = false; //program uses "pthread.h"

    bool includes; //program uses includes instead of declarations for standard library functions, for testing purposes only
    bool noFuncCasts; //program removes any function call casts, for testing purposes only

    /**
     * @brief Program Constructor of a Program class, parses given file into a llvm::Module.
     * @param file Path to a file for parsing.
     * @param includes Program uses includes instead of declarations.
     * @param casts Program removes function call casts.
     */
    //Program(const std::string& file, bool includes, bool casts);
    Program();

    /**
     * @brief getStruct Returns pointer to the Struct corresponding to the given LLVM StructType.
     * @param strct LLVM StructType
     * @return Pointer to Struct expression if the struct is found, nullptr otherwise
     */
    Struct* getStruct(const llvm::StructType* strct) const;

    /**
     * @brief getStruct Returns pointer to the Struct with the given name.
     * @param name Name of the struct
     * @return Pointer to Struct expression if the struct is found, nullptr otherwise
     */
    Struct* getStruct(const std::string& name) const;

    /**
     * @brief getGlobalVar Returns corresponding refference to GlobalValue expression.
     * @param val llvm global variable
     * @return RefExpr expression or nullptr
     */
    RefExpr* getGlobalVar(const llvm::Value* val);

    /**
     * @brief getFunction Returns corresponding function to LLVM function.
     * @param f llvm function pointer
     * @return Func function or nullptr
     */
    Func* getFunction(const llvm::Function* f);

    /**
     * @brief addDeclaration Adds new declaration of given function.
     * @param func LLVM Function
     */
    void addDeclaration(const llvm::Function* func, std::unique_ptr<Func> decl);

    /**
     * @brief getType Transforms llvm::Type into corresponding Type object
     * @param type llvm::Type for transformation
     * @return unique_ptr to corresponding Type object
     */
    std::unique_ptr<Type> getType(const llvm::Type* type);

    RefExpr* getGlobalRef(const llvm::GlobalVariable* gv);

    void addFunction(const llvm::Function* llvmFunc, std::unique_ptr<Func> func);

    bool isFunctionDeclared(const llvm::Function* func) const;

    /**
     * @brief getVarName Creates a new name for a variable in form of string containing "var" + structVarCount.
     * @return String containing a new variable name.
     */
    std::string getStructVarName();

    void addStruct(std::unique_ptr<Struct> strct);

    const std::set<std::string>& getGlobalVarNames() const;

    Func* getDeclaration(const llvm::Function* func);
};
