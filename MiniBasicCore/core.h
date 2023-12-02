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
    QVector<QString> codeHistory;//有序存储执行成功的所有代码
public:
    Core();
    int exec(int argc, char *argv[]);
private:
    //将中缀表达式转成后缀表达式，输出到os
    ostream& Infix2Suffix(ostream& os,const string& infix);
    int32_t parseExpression(const string& str);
private:
    void printVarTable() const;
    void printCodeHistory() const;
};

#endif // CORE_H
