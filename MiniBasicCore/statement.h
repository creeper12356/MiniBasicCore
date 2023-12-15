#ifndef STATEMENT_H
#define STATEMENT_H
#include "inc.h"
#include "expression.h"
//运行次数结构体
union RunTime{
    int count;
    struct {
        int trueCount;
        int falseCount;
    }conditionCount;
};

class Statement{
public:
    //解析字符串语句自动解析生成语句,
    //lineNumCheck表示是否检查行号范围
    static Statement* newStatement(const QString &src,bool lineNumCheck = true);
    //打印错误的语法树
    static void printErrSyntaxTree(QTextStream& out);

    Statement(int lineNum, StatementType type, const QString &source,
              Exception buildException = NoException);
    virtual ~Statement(){}
public:
    virtual int exec(Context* context) = 0;
    virtual void printSyntaxTree(QTextStream& out) const = 0;
    virtual void updateRunTime(bool flag = true);
    virtual void printRunTime() const;

    int getLineNum() const{return _lineNum;}
    const StatementType& getType() const{return _type;}
    const QString& getSource() const{return _source;}
    Exception getBuildException() const {return _buildException;}

    void clearRunTime();
protected:
    int _lineNum;
    StatementType _type;
    QString _source;
    RunTime _runTime;
    //构造函数调用时发生的异常，构造时不报错，运行时报错
    Exception _buildException = NoException;
};

class ErrStatement: public Statement{
public:
    ErrStatement(int lineNum, const QString& source, Exception buildException);
    int exec(Context *context) override;
    void printSyntaxTree(QTextStream& out) const override;
};

class RemStatement: public Statement{
private:
    //注释内容
    QString _comment;
public:
    RemStatement(int lineNum, const QString &source, const QStringList& argList);
    int exec(Context* context) override;
    void printSyntaxTree(QTextStream& out) const override;
};

class LetStatement: public Statement{
private:
    Expression* _leftExpr = nullptr;
    Expression* _rightExpr = nullptr;
public:
    LetStatement(int lineNum , const QString& source, const QStringList& argList);
    ~LetStatement() override;
    int exec(Context* context) override;
    void printSyntaxTree(QTextStream& out) const override;
};

class PrintStatement: public Statement{
private:
    Expression* _expr = nullptr;
public:
    PrintStatement(int lineNum,const QString& source,const QStringList& argList);
    ~PrintStatement() override;
    int exec(Context* context) override;
    void printSyntaxTree(QTextStream& out) const override;
};

class InputStatement: public Statement{
private:
    Expression* _expr = nullptr;
public:
    InputStatement(int lineNum,const QString& source,const QStringList& argList);
    ~InputStatement() override;
    int exec(Context* context) override;
    void printSyntaxTree(QTextStream& out) const override;
};

class GotoStatement: public Statement{
private:
    int _destination;
public:
    GotoStatement(int lineNum,const QString& source,const QStringList& argList);
    int exec(Context* context) override;
    void printSyntaxTree(QTextStream& out) const override;
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
    int exec(Context* context) override;
    void printSyntaxTree(QTextStream& out) const override;
    void updateRunTime(bool flag) override;
    void printRunTime() const override;
};

class EndStatement: public Statement{
public:
    EndStatement(int lineNum,const QString& source);
    int exec(Context* context) override;
    void printSyntaxTree(QTextStream& out) const override;
};

#endif // STATEMENT_H
