#ifndef CONTEXT_H
#define CONTEXT_H
#include "inc.h"
class Context
{
    friend class RemStatement;
    friend class LetStatement;
    friend class PrintStatement;
    friend class InputStatement;
    friend class GotoStatement;
    friend class IfStatement;
    friend class EndStatement;
    friend class Expression;
public:
    Context();
    ~Context();
    
    void appendCode(const QString& code);
    void clearCodes();
    void clearRunningStatus();
    void listCodes() const;
    void analyze(QTextStream& out, bool isFormat) const;
    void printVarTable() const;
    void printRunTime() const;
    void printUseCount() const;
    int executeCode(Statement* code);
    int getPC() const;
    void runCodes();
private:
    void gotoLine(int dst);
    
    QMap<QString,int32_t> varTable;
    QMap<QString,int> useCount;
    QVector<Statement*> codes;
    int PC;
};

#endif // CONTEXT_H
