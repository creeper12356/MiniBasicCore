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
    QVector<QString> codes;
    //指向下一条指令
    int PC;
public:
    Core();
    ~Core();
    int exec(int argc, char *argv[]);
private:
    //执行指令code，
    //成功并准备下一次执行返回1，
    //成功并退出返回0，
    //失败抛出异常
    int exeCode(const QString &code);
    //将PC设为dst
    void gotoLine(int dst);
private:
    void printVarTable() const;
};

#endif // CORE_H
