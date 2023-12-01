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
        throw ParseError;
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
