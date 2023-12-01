#ifndef CORE_H
#define CORE_H
#include <iostream>
#include <string>
#include <sstream>
#include <ostream>

#include <QVector>
#include <QStack>

#include "statement.h"
#include "expression.h"
using namespace std;

//QVector<Op> operators = {Op("+",1),Op("-",1),Op("*",2),Op("/",2)};
class Core
{
private:
    QVector<Var> variables = {};
public:
    Core();
    int exec();
private:
    //将中缀表达式转成后缀表达式，输出到os
    ostream& Infix2Suffix(ostream& os,const string& infix);
    qint32 parseExpression(const string& str);
};

#endif // CORE_H
