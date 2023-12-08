#ifndef CORE_H
#define CORE_H
#include <iostream>
#include <string>
#include <sstream>
#include <ostream>

#include <QVector>
#include <QStack>
#include <QMap>

#include "statement.h"
#include "expression.h"
using namespace std;
enum read_mode{read_other = 0,read_digit,read_var};
class Core
{
private:
    QMap<string,int32_t> varTable;//变量表
    QVector<QString> codes;//TODO

    //指向codeHistory中下一条指令
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
    int exeCode(const string& code);
    //将中缀表达式转成后缀表达式，输出到os
    ostream& infix2Suffix(ostream& os,const string& infix);
    //解析后缀表达式，返回运算结果
    int32_t parseSuffixExpr(const string& expr);
    //解析中缀表达式，返回运算结果
    int32_t parseInfixExpr(const string& expr);
    //解析简单的条件表达式
    bool parseBoolExpr(const string& expr);
    //改变PC
    void gotoLine(int dst);
private:
    void printVarTable() const;
};

#endif // CORE_H
