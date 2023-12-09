#ifndef STATEMENT_H
#define STATEMENT_H
#include "inc.h"

class Statement{
public:
    static Statement* newStatement(const QString &src);

    Statement(int lineNum, StatementType type, const QString &source);
    virtual ~Statement(){}
    virtual int exec(Core* context) = 0;
    int getLineNum() const{return _lineNum;}
    const StatementType& getType() const{return _type;}
    const QString& getSource() const{return _source;}
protected:
    int _lineNum;
    StatementType _type;
    QString _source;
};

class RemStatement: public Statement{
private:
    //注释内容
    QString _comment;
public:
    RemStatement(int lineNum, const QString &source, const QStringList& argList);
    int exec(Core* context) override;
};

class LetStatement: public Statement{
private:
    QString _left;
    QString _right;
public:
    LetStatement(int lineNum , const QString& source, const QStringList& argList);
    int exec(Core* context) override;
};

class PrintStatement: public Statement{
private:
    QString _expr;
public:
    PrintStatement(int lineNum,const QString& source,const QStringList& argList);
    int exec(Core* context) override;
};

class InputStatement: public Statement{
private:
    QString _expr;
public:
    InputStatement(int lineNum,const QString& source,const QStringList& argList);
    int exec(Core* context) override;
};

class GotoStatement: public Statement{
private:
    int _destination;
public:
    GotoStatement(int lineNum,const QString& source,const QStringList& argList);
    int exec(Core* context) override;
};

class IfStatement:public Statement{
private:
    QString _condition;
    int _destination;
public:
    IfStatement(int lineNum, const QString& source, QStringList argList);
    int exec(Core* context) override;
};

class EndStatement: public Statement{
public:
    EndStatement(int lineNum,const QString& source);
    int exec(Core* context) override;
};

#endif // STATEMENT_H
