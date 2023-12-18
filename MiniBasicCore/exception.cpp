#include "exception.h"

QString Exception::toString() const
{
    switch (this->type){
    case ParseError: return "解析错误";
    case WrongLeftValue: return "非法的左值";
    case WrongAssignSyntax: return "错误的赋值语法";
    case WrongCmpSyntax: return info.isEmpty() ? "错误的比较语法" : info;
    case WrongIfSyntax: return info.isEmpty() ? "错误的条件语法" : info;
    case WrongVarName: return info.isEmpty() ? "非法变量名" : QString("非法变量名:'%1'").arg(info);
    case EmptyExpr: return "空表达式";
    case BracketsNotMatch: return "括号不匹配";
    case UnknownOp: return QString("未知运算符'%1'").arg(info);
    case UseBeforeDeclare: return QString("变量'%1'未定义").arg(info);
    case NoLineNum: return "找不到行号";
    case WrongLineNum: return "错误的行号";
    case UnknownStatementType: return QString("未知语句类型'%1'").arg(info);
    case WrongGotoDst: return QString("错误的跳转地址'%1'").arg(info);
    }
}

std::string Exception::toStdString() const
{
    return toString().toStdString();
}
