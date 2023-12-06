#ifndef STATEMENT_H
#define STATEMENT_H
#include <string>
#include <iostream>
#include <QString>
#include <QStringList>

typedef QString StatementType;
enum Exception{
    ParseError,
    WrongAssignSyntax,//错误的赋值语法
    WrongCmpSyntax,//错误的比较语法
    WrongIfSyntax,//错误的if...then语法
    WrongVarName,//非法变量将名
    EmptyExpr,//空表达式
    BracketsNotMatch,//括号不匹配
    UnknownOp,//未知运算符
    UseBeforeDeclare,
    NoLineNum,
    WrongLineNum,	//行号要求位于1-1000,000之间
    UnknownStatementType,
    WrongGotoDst,
};

class Statement{
public:
    Statement(const QString& src);
    Statement(const Statement &other);
public:
    int lineNum() const{return _lineNum;}
    const StatementType& type() const{return _type;}
    const QString& source() const{return _source;}
    const QString& object() const{return _object;}
protected:
    int _lineNum;
    StatementType _type;
    QString _source;
    QString _object;
};
class IfStatement:public Statement{
public:
    const QString& condition() const{return _condition;}
    int destination() const{return _destination;}
private:
    QString _condition;
    int _destination;
public:
    IfStatement(const Statement& stmt);
};

#endif // STATEMENT_H
