#include "statement.h"
#include "core.h"

Statement *Statement::newStatement(const QString &src)
{
    QStringList argList = src.split(" " ,QString::SkipEmptyParts);
    int size = argList.size();
    if(size == 0){
        //空行
        return nullptr;
    }
    //assert size >= 1
    bool isNum;
    int lineNum = argList[0].toInt(&isNum);
    if(!isNum){
        //第一个参数不是行号
        throw NoLineNum;
    }
    if(lineNum < 0 || lineNum > 1000000){
        //行号范围错误
        throw WrongLineNum;
    }

    if(size == 1){
        //只有行号
        return nullptr;
    }
    //assert(size >= 2)
    StatementType type = argList[1];
    //去除行号和类型
    argList.removeFirst();
    argList.removeFirst();
    //替换余数运算符
    for(auto it = argList.begin(); it != argList.end();++it){
        if(*it == "MOD"){
            argList.replace(it - argList.begin(),"%");
        }
    }
    if(type == "REM"){
        return new RemStatement(lineNum,src,argList);
    }
    else if(type == "LET"){
        return new LetStatement(lineNum,src,argList);
    }
    else if(type == "PRINT"){
        return new PrintStatement(lineNum,src,argList);
    }
    else if(type == "INPUT"){
        return new InputStatement(lineNum,src,argList);
    }
    else if(type == "GOTO"){
        return new GotoStatement(lineNum,src,argList);
    }
    else if(type == "IF"){
        return new IfStatement(lineNum,src,argList);
    }
    else if(type == "END"){
        return new EndStatement(lineNum,src);
    }
    else{
        //未知语句类型
        throw UnknownStatementType;
    }

}

Statement::Statement(int lineNum, StatementType type, const QString &source)
    :_lineNum(lineNum)
    ,_type(type)
    ,_source(source)
{
}



RemStatement::RemStatement(int lineNum, const QString& source, const QStringList &argList)
    :Statement(lineNum,"REM",source)
    ,_comment(argList.join(" "))
{
}

int RemStatement::exec(Core *context)
{
    context->PC += 1;
    return 1;
}


LetStatement::LetStatement(int lineNum, const QString &source, const QStringList &argList)
    :Statement(lineNum,"LET",source)
{
    QString obj = argList.join("");
    int assignmentIndex = -1;
    for(auto it = obj.begin();it != obj.end();++it){
        if(*it == '='){
            assignmentIndex = it - obj.begin();
            break;
        }
    }
    if(assignmentIndex == -1){
        //没有赋值号
        throw WrongAssignSyntax;
    }
    _left = obj.left(assignmentIndex);
    _right = obj.right(obj.size() - assignmentIndex - 1);
//    先保证解析正确再赋值
//    int parseRes = parseInfixExpr(right);
    //    varTable[left] = parseRes;
}

int LetStatement::exec(Core *context)
{
    //TODO : exception handler

    //先解析,解析无误后再赋值
    int32_t parseRes = context->parseInfixExpr(_right);
    context->varTable[_left] = parseRes;
    context->PC += 1;
    return 1;
}

PrintStatement::PrintStatement(int lineNum, const QString &source, const QStringList &argList)
    :Statement(lineNum,"PRINT",source)
    ,_expr(argList.join(""))
{
}

int PrintStatement::exec(Core *context)
{
    std::cout << context->parseInfixExpr(_expr) << std::endl;
    context->PC += 1;
    return 1;
}

InputStatement::InputStatement(int lineNum, const QString &source, const QStringList &argList)
    :Statement(lineNum,"INPUT",source)
    ,_expr(argList.join(""))
{
}

int InputStatement::exec(Core *context)
{
    //TODO 检查destination合法性
    std::cout << "?";
    int32_t input;
    //TODO 前端保证，用户只能输入一个整数
    std::cin >> input;
    std::cin.get();
    context->varTable[_expr] = input;
    context->PC += 1;
    return 1;
}
IfStatement::IfStatement(int lineNum,const QString& source,QStringList argList)
    :Statement(lineNum,"IF",source)
{
    //assert argList.size() >= 2
    if(argList.size() < 2){
        throw WrongIfSyntax;
    }
    if(*(argList.end() - 2) != "THEN"){
        //找不到合法的THEN子句
        throw WrongIfSyntax;
    }

    bool isNum = false;
    _destination = argList.last().toInt(&isNum);
    if(!isNum){
        throw WrongGotoDst;
    }
    //去除THEN 和 destination
    argList.removeLast();
    argList.removeLast();
    if(argList.empty()){
        //空条件
        throw EmptyExpr;
    }
    //合成条件表达式
    _condition = argList.join("");
}

int IfStatement::exec(Core *context)
{
    if(context->parseBoolExpr(_condition)){
        context->gotoLine(_destination);
    }
    else{
        context->PC += 1;
    }
    return 1;
}
GotoStatement::GotoStatement(int lineNum, const QString &source, const QStringList &argList)
    :Statement(lineNum,"GOTO",source)
{
    bool isNum = false;
    //assert argList.size == 1
    if(argList.size() != 1){
        throw WrongGotoDst;
    }
    _destination = argList.first().toInt(&isNum);
    if(!isNum){
        throw WrongGotoDst;
    }
}

int GotoStatement::exec(Core *context)
{
    context->gotoLine(_destination);
    return 1;
}

EndStatement::EndStatement(int lineNum, const QString &source)
    :Statement(lineNum,"END",source)
{
}

int EndStatement::exec(Core *context)
{
    context->PC += 1;
    return 0;
}
