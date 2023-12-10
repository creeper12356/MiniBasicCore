#ifndef STATEMENT_H
#define STATEMENT_H
#include "inc.h"
#include "expression.h"
class Statement{
public:
    static Statement* newStatement(const QString &src);

    Statement(int lineNum, StatementType type, const QString &source, Exception buildException = NoException);
    virtual ~Statement(){}
    virtual int exec(Core* context) = 0;
    virtual void printSyntaxTree() const = 0;
    int getLineNum() const{return _lineNum;}
    const StatementType& getType() const{return _type;}
    const QString& getSource() const{return _source;}
    Exception getBuildException() const {return _buildException;}
protected:
    int _lineNum;
    StatementType _type;
    QString _source;
    //构造函数调用时发生的异常，构造时不报错，运行时报错
    Exception _buildException = NoException;
};

class ErrStatement: public Statement{
public:
    ErrStatement(const QString& source, Exception buildException);
    int exec(Core *context) override;
    void printSyntaxTree() const override;
};

class RemStatement: public Statement{
private:
    //注释内容
    QString _comment;
public:
    RemStatement(int lineNum, const QString &source, const QStringList& argList);
    int exec(Core* context) override;
    void printSyntaxTree() const override;
};

class LetStatement: public Statement{
private:
    Expression* _leftExpr = nullptr;
    Expression* _rightExpr = nullptr;
public:
    LetStatement(int lineNum , const QString& source, const QStringList& argList);
    ~LetStatement() override;
    int exec(Core* context) override;
    void printSyntaxTree() const override;
};

class PrintStatement: public Statement{
private:
    Expression* _expr = nullptr;
public:
    PrintStatement(int lineNum,const QString& source,const QStringList& argList);
    ~PrintStatement() override;
    int exec(Core* context) override;
    void printSyntaxTree() const override;
};

class InputStatement: public Statement{
private:
    Expression* _expr = nullptr;
public:
    InputStatement(int lineNum,const QString& source,const QStringList& argList);
    ~InputStatement() override;
    int exec(Core* context) override;
    void printSyntaxTree() const override;
};

class GotoStatement: public Statement{
private:
    int _destination;
public:
    GotoStatement(int lineNum,const QString& source,const QStringList& argList);
    int exec(Core* context) override;
    void printSyntaxTree() const override;
};

class IfStatement:public Statement{
private:
    Expression* _conditionLeft = nullptr;
    Expression* _conditionRight = nullptr;
    QChar _conditionOp;
    int _destination;
public:
    IfStatement(int lineNum, const QString& source, QStringList argList);
    ~IfStatement() override;
    int exec(Core* context) override;
    void printSyntaxTree() const override;
};

class EndStatement: public Statement{
public:
    EndStatement(int lineNum,const QString& source);
    int exec(Core* context) override;
    void printSyntaxTree() const override;
};

#endif // STATEMENT_H
