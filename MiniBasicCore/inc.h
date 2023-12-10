#ifndef INC_H
#define INC_H
#include <QProcess>
#include <QDebug>
#include <QVector>
#include <QStack>
#include <QMap>
#include <QString>
#include <QStringList>

#include <iostream>
#include <string>
typedef QString StatementType;
enum Exception{
    NoException, //没有异常
    ParseError, //解析错误
    WrongLeftValue, //非法的左值
    WrongAssignSyntax,//错误的赋值语法
    WrongCmpSyntax,//错误的比较语法
    WrongIfSyntax,//错误的if...then语法
    WrongVarName,//非法变量名
    EmptyExpr,//空表达式
    BracketsNotMatch,//括号不匹配
    UnknownOp,//未知运算符
    UseBeforeDeclare,
    NoLineNum,
    WrongLineNum,	//行号要求位于1-1000,000之间
    UnknownStatementType,
    WrongGotoDst,
};
enum read_mode{read_other = 0,read_digit,read_var};
class Core;
class Statement;
#endif // INC_H
