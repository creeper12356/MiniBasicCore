#include "statement.h"
#include "core.h"

Statement *Statement::newStatement(const QString &src)
{
    try{
        QStringList argList = src.split(" " ,QString::SkipEmptyParts);
        int size = argList.size();
        if(size == 0){
            //空行
            //never occur
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
            //never occur in frontend
            throw EmptyExpr;
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
    catch(Exception e){
        //处理基本构造失败的语句
        return new ErrStatement(src,e);
    }
}

void Statement::printErrSyntaxTree()
{
    std::cout << "Error" << std::endl;
}

Statement::Statement(int lineNum, StatementType type, const QString &source,Exception buildException)
    :_lineNum(lineNum)
    ,_type(type)
    ,_source(source)
    ,_buildException(buildException)
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

void RemStatement::printSyntaxTree() const
{
    std::cout << _lineNum << " REM" << std::endl;
    std::cout << "\t" << _comment.toStdString() << std::endl;
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
    try{
        if(assignmentIndex == -1){
            //没有赋值号
            throw WrongAssignSyntax;
        }
        QString left = obj.left(assignmentIndex);
        QString right = obj.right(obj.size() - assignmentIndex - 1);

        _leftExpr = new Expression(Expression::infix2Suffix(left));
        _rightExpr = new Expression(Expression::infix2Suffix(right));
    }
    catch(Exception e){
        //构造赋值语句失败
        _buildException = e;
        if(_leftExpr){
            delete _leftExpr; _leftExpr = nullptr;
        }
        if(_rightExpr){
            delete _rightExpr; _rightExpr = nullptr;
        }
    }
}

LetStatement::~LetStatement()
{
    if(_leftExpr) delete _leftExpr;
    if(_rightExpr) delete _rightExpr;
}

int LetStatement::exec(Core *context)
{
    //TODO : exception handler

    if(_leftExpr->getType() != exp_var){
        //左值不是变量
        throw WrongLeftValue;
    }
    //先解析,解析无误后再赋值
    int32_t parseRes = _rightExpr->value(context);
    context->varTable[_leftExpr->getRootData()] = parseRes;
    context->PC += 1;
    return 1;
}

void LetStatement::printSyntaxTree() const
{
    if(_buildException != NoException){
        Statement::printErrSyntaxTree();
        return ;
    }
    std::cout << _lineNum << " LET = " << std::endl;
    _leftExpr->printExpTree(1);
    _rightExpr->printExpTree(1);
}

PrintStatement::PrintStatement(int lineNum, const QString &source, const QStringList &argList)
    :Statement(lineNum,"PRINT",source)
{
    try{
        _expr = new Expression(Expression::infix2Suffix(argList.join("")));
    }
    catch(Exception e){
        //构造打印语句失败
        //assert(_expr == nullptr)
        _buildException = e;
    }
}

PrintStatement::~PrintStatement()
{
    if(_expr) delete _expr;
}

int PrintStatement::exec(Core *context)
{
    std::cout << _expr->value(context) << std::endl;
    context->PC += 1;
    return 1;
}

void PrintStatement::printSyntaxTree() const
{
    if(_buildException != NoException){
        Statement::printErrSyntaxTree();
        return ;
    }
    std::cout << _lineNum << " PRINT" << std::endl;
    _expr->printExpTree(1);
}

InputStatement::InputStatement(int lineNum, const QString &source, const QStringList &argList)
    :Statement(lineNum,"INPUT",source)
{
    try{
        _expr = new Expression(Expression::infix2Suffix(argList.join("")));
        if(_expr->getType() != exp_var){
            //检查左值合法性
            throw WrongLeftValue;
        }
    }
    catch(Exception e){
        //构造输入语句失败
        if(_expr) {
            delete _expr; _expr = nullptr;
        }
        _buildException = e;
    }
}

InputStatement::~InputStatement()
{
    if(_expr) delete _expr;
}

int InputStatement::exec(Core *context)
{
    //assert _expr.type == exp_var
    std::cout << "?";
    int32_t input;
    //TODO 前端保证，用户只能输入一个整数
    std::cin >> input;
    std::cin.get();
    context->varTable[_expr->getRootData()] = input;
    context->PC += 1;
    return 1;
}

void InputStatement::printSyntaxTree() const
{
    if(_buildException != NoException){
        Statement::printErrSyntaxTree();
        return ;
    }
    std::cout << _lineNum << " INPUT" << std::endl;
    _expr->printExpTree(1);
}
IfStatement::IfStatement(int lineNum,const QString& source,QStringList argList)
    :Statement(lineNum,"IF",source)
{
    try{
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
        if(_destination < 0 || _destination > 1000000){
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
        QString condition = argList.join("");
        //TODO conditon表达式解析
        auto it = condition.begin();
        for(;it != condition.end();++it){
            if(*it == '<' || *it == '=' || *it == '>'){
                _conditionOp = *it;
                break;
            }
        }
        if(it == condition.end()){
            //找不到比较运算符('<','=','>')
            throw WrongCmpSyntax;
        }
        _conditionLeft = new Expression(Expression::infix2Suffix(condition.left(it - condition.begin())));
        _conditionRight = new Expression(Expression::infix2Suffix(condition.right(condition.end() - 1 - it)));
    }
    catch(Exception e){
        //条件语句构造异常
        if(_conditionLeft){
            delete _conditionLeft; _conditionLeft = nullptr;
        }
        if(_conditionRight){
            delete _conditionRight; _conditionRight = nullptr;
        }
        _buildException = e;
    }
}

IfStatement::~IfStatement()
{
    if(_conditionLeft) delete _conditionLeft;
    if(_conditionRight) delete _conditionRight;
}

int IfStatement::exec(Core *context)
{
    //解析布尔表达式
    bool parseRes;
    if(_conditionOp == '>'){
        parseRes = _conditionLeft->value(context) > _conditionRight->value(context);
    }
    else if(_conditionOp == '='){
        parseRes = _conditionLeft->value(context) == _conditionRight->value(context);
    }
    else if(_conditionOp == '<'){
        parseRes = _conditionLeft->value(context) < _conditionRight->value(context);
    }

    //条件跳转
    if(parseRes){
        context->gotoLine(_destination);
    }
    else{
        context->PC += 1;
    }
    return 1;
}

void IfStatement::printSyntaxTree() const
{
    if(_buildException != NoException){
        Statement::printErrSyntaxTree();
        return ;
    }
    std::cout << _lineNum << " IF THEN" << std::endl;
    _conditionLeft->printExpTree(1);
    std::cout << '\t' << _conditionOp.toLatin1() << std::endl;
    _conditionRight->printExpTree(1);
    std::cout << '\t' << _destination << std::endl;
}
GotoStatement::GotoStatement(int lineNum, const QString &source, const QStringList &argList)
    :Statement(lineNum,"GOTO",source)
{
    bool isNum = false;
    //assert argList.size == 1
    try{
        if(argList.size() != 1){
            throw WrongGotoDst;
        }
        _destination = argList.first().toInt(&isNum);
        if(!isNum){
            throw WrongGotoDst;
        }
        if(_destination < 0 || _destination > 1000000){
            throw WrongGotoDst;
        }
    }
    catch(Exception e){
        _buildException = e;
    }
}

int GotoStatement::exec(Core *context)
{
    context->gotoLine(_destination);
    return 1;
}

void GotoStatement::printSyntaxTree() const
{
    if(_buildException != NoException){
        Statement::printErrSyntaxTree();
        return ;
    }
    std::cout << _lineNum << " GOTO " << _destination << std::endl;
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

void EndStatement::printSyntaxTree() const
{
    std::cout << _lineNum << " END" << std::endl;
}

ErrStatement::ErrStatement(const QString &source, Exception buildException)
    :Statement(-1,"ERR",source,buildException)
{
}

int ErrStatement::exec(Core *context)
{
    //永远不会被调用
    return 0;
}

void ErrStatement::printSyntaxTree() const
{
    Statement::printErrSyntaxTree();
}
