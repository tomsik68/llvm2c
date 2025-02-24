#pragma once

class Struct;
class StructElement;
class ArrayElement;
class ExtractValueExpr;
class Value;
class GlobalValue;
class IfExpr;
class SwitchExpr;
class AsmExpr;
class CallExpr;
class PointerShift;
class GepExpr;
class SelectExpr;
class RefExpr;
class DerefExpr;
class RetExpr;
class CastExpr;
class AddExpr;
class SubExpr;
class AssignExpr;
class MulExpr;
class DivExpr;
class RemExpr;
class AndExpr;
class OrExpr;
class XorExpr;
class CmpExpr;
class AshrExpr;
class LshrExpr;
class ShlExpr;
class StackAlloc;

class ExprVisitor {
public:
    virtual void visit(Struct& expr) {}
    virtual void visit(StructElement& expr) {}
    virtual void visit(ArrayElement& expr) {}
    virtual void visit(ExtractValueExpr& expr) {}
    virtual void visit(Value& expr) {}
    virtual void visit(GlobalValue& expr) {}
    virtual void visit(IfExpr& expr) {}
    virtual void visit(SwitchExpr& expr) {}
    virtual void visit(AsmExpr& expr) {}
    virtual void visit(CallExpr& expr) {}
    virtual void visit(PointerShift& expr) {}
    virtual void visit(GepExpr& expr) {}
    virtual void visit(SelectExpr& expr) {}
    virtual void visit(RefExpr& expr) {}
    virtual void visit(DerefExpr& expr) {}
    virtual void visit(RetExpr& expr) {}
    virtual void visit(CastExpr& expr) {}
    virtual void visit(AddExpr& expr) {}
    virtual void visit(SubExpr& expr) {}
    virtual void visit(AssignExpr& expr) {}
    virtual void visit(MulExpr& expr) {}
    virtual void visit(DivExpr& expr) {}
    virtual void visit(RemExpr& expr) {}
    virtual void visit(AndExpr& expr) {}
    virtual void visit(OrExpr& expr) {}
    virtual void visit(XorExpr& expr) {}
    virtual void visit(CmpExpr& expr) {}
    virtual void visit(AshrExpr& expr) {}
    virtual void visit(LshrExpr& expr) {}
    virtual void visit(ShlExpr& expr) {}
    virtual void visit(StackAlloc& expr) {}
    virtual ~ExprVisitor() = default;
};
