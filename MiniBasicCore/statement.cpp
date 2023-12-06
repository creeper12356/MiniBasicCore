#include "statement.h"

Statement::Statement(const QString& src){
    //按空格分割源，忽略多余空白字符
    //只能忽略空格，无法忽略tab，需要在前端禁止tab输入
    QStringList parseList = src.split(" ",QString::SkipEmptyParts);
    int size = parseList.size();
    if(size == 0){
        //空行
        //空行行号先标记成0
        _lineNum = 0;
        _type = "REM";
        return ;
    }
    bool isNum;
    _lineNum = parseList[0].toInt(&isNum);
    if(!isNum){
        //第一个参数不是行号
        throw NoLineNum;
    }
    if(_lineNum < 0 || _lineNum > 1000000){
        throw WrongLineNum;
    }

    if(size == 1){
        //只有行号
        //TODO: 先默认合法
        _type = "REM";
        return ;
    }

    _type = parseList[1];
    _source = src;
    //去除行号和指令关键字
    parseList.removeFirst();
    parseList.removeFirst();

    //替换余数运算符
    for(int i = 0;i < parseList.size();++i){
        if(parseList[i] == "MOD"){
            std::cout << "replace.";
            parseList.replace(i,"%");
        }
    }
    //处理复杂运算符
    _object = parseList.join("");
}

Statement::Statement(const Statement& other):
    _lineNum(other.lineNum()),
    _type(other.type()),
    _source(other.source()),
    _object(other.object())
{
}

IfStatement::IfStatement(const Statement &stmt)
    :Statement(stmt)
{
    //assert parseList.size() >= 2
    //TODO: 避免重复解析
    QStringList parseList = _source.split(" ",QString::SkipEmptyParts);
    if(parseList[parseList.size() - 2] != "THEN"){
        //找不到合法的THEN子句
        throw WrongIfSyntax;
    }
    bool isNum = false;
    _destination = parseList.last().toInt(&isNum);
    if(!isNum){
        throw WrongGotoDst;
    }
    //去除THEN 和 destination
    parseList.removeLast();
    parseList.removeLast();
    //去除行号和IF
    parseList.removeFirst();
    parseList.removeFirst();
    if(parseList.empty()){
        std::cerr << "Empty contition detected.\n";
        throw EmptyExpr;
    }
    //合成条件表达式
    _condition = parseList.join("");
//    std::cout << "condition: " << _condition.toStdString() << std::endl;
//    std::cout << "destination: " << _destination << std::endl;
}
