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
    void analyze() const;
    void runTime() const;
    int executeCode(Statement* code);
    void runCodes();
private:
    void gotoLine(int dst);
    
    QMap<QString,int32_t> varTable;
    QVector<Statement*> codes;
    int PC;
};


#endif // CONTEXT_H
