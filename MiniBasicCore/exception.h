#ifndef EXCEPTION_H
#define EXCEPTION_H
//本项目中所有的异常类
#include <QString>
enum exception_type{
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

class Exception{
public:
    exception_type type;
    QString info;

    Exception(exception_type arg_t = NoException, QString arg_i = "")
        :type(arg_t),info(arg_i){}
    //将异常转换为可读字符串
    QString toString() const;
    std::string toStdString() const;
};

#endif // EXCEPTION_H
