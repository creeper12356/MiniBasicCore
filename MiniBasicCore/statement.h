#ifndef STATEMENT_H
#define STATEMENT_H
#include <string>
#include <iostream>
#include <QString>
#include <QStringList>

typedef QString StatementType;
enum Exception{
    ParseError,
    UseBeforeDeclare,
    NoLineNum,
    WrongLineNum,	//行号要求位于1-1000,000之间
    UnknownStatementType
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
