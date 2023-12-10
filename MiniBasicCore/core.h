#ifndef CORE_H
#define CORE_H
#include "inc.h"
using namespace std;
class Core
{
    friend class RemStatement;
    friend class LetStatement;
    friend class PrintStatement;
    friend class InputStatement;
    friend class GotoStatement;
    friend class IfStatement;
    friend class EndStatement;
    friend class Expression;
private:
    QMap<QString,int32_t> varTable;//变量表
    QVector<Statement*> codes;
    //指向下一条指令
    int PC = 0;
public:
    Core();
    ~Core();
    int exec(int argc, char *argv[]);
private:
    //将PC设为dst
    void gotoLine(int dst);
private:
    void printVarTable() const;
};

#endif // CORE_H
