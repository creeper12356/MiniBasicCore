#ifndef STATEMENT_H
#define STATEMENT_H
#include <QString>
#include <QStringList>

typedef QString StatementType;
enum Exception{
    ParseError = 0, //解析错误
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
class Core;
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
