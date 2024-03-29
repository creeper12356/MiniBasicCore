#include "statement.h"
#include "context.h"

Statement *Statement::newStatement(Context* context , QString src, bool lineNumCheck)
{
    int lineNum = -1;
    //将乘方运算符转化成单字符表示，便于处理
    src.replace("**" , "^");
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
        lineNum = argList[0].toInt(&isNum);
        if(!isNum){
            //第一个参数不是行号
            lineNum = -1;
            throw Exception(NoLineNum);
        }
        if(lineNumCheck && (lineNum < 1 || lineNum > 1000000)){
            //行号范围错误
            lineNum = -1;
            throw Exception(WrongLineNum,argList.first());
        }

        if(size == 1){
            //只有行号
            //never occur in frontend
            throw Exception(EmptyExpr);
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
            return new RemStatement(context,lineNum,src,argList);
        }
        else if(type == "LET"){
            return new LetStatement(context ,lineNum,src,argList);
        }
        else if(type == "PRINT"){
            return new PrintStatement(context , lineNum,src,argList);
        }
        else if(type == "INPUT"){
            return new InputStatement(context , lineNum,src,argList);
        }
        else if(type == "GOTO"){
            return new GotoStatement(context , lineNum,src,argList);
        }
        else if(type == "IF"){
            return new IfStatement(context , lineNum,src,argList);
        }
        else if(type == "END"){
            return new EndStatement(context , lineNum,src);
        }
        else{
            //未知语句类型
            throw Exception(UnknownStatementType,type);
        }
    }
    catch(Exception e){
        //处理基本构造失败的语句
        return new ErrStatement(context , lineNum,src,e);
    }
}

void Statement::printErrSyntaxTree(QTextStream& out , bool isFormat) const
{
    out << getLineNumStr() << " Error " ;
    printFormatRunTime(out , _runTime.count , isFormat) << endl;
}

Statement::Statement(Context *context, int lineNum, StatementType type, const QString &source, Exception buildException)
    :_context(context)
    ,_lineNum(lineNum)
    ,_type(type)
    ,_source(source)
    ,_buildException(buildException)
{
    clearRunTime();
}

void Statement::updateRunTime(bool flag)
{
    ++_runTime.count;
}

void Statement::printRunTime() const
{
    std::cout << _runTime.count << std::endl;
}

QString Statement::getLineNumStr() const
{
    if(_lineNum == -1){
        return "?";
    }
    else if(_lineNum == 0){
        return "脚本";
    }
    else {
        return QString::number(_lineNum);
    }
}

void Statement::clearRunTime()
{
    _runTime.conditionCount.trueCount = _runTime.conditionCount.falseCount = 0;
}

QTextStream &Statement::printFormatRunTime(QTextStream &out, int runTime, bool isFormat)
{
    if(isFormat) out << "[" << runTime << "]" ;
    else out << runTime ;
    return out;
}

RemStatement::RemStatement(Context* context, int lineNum, const QString& source, const QStringList &argList)
    :Statement(context , lineNum ,"REM",source)
    ,_comment(argList.join(" "))
{
}

int RemStatement::exec()
{
    _context->PC += 1;
    updateRunTime();
    return 1;
}

void RemStatement::printSyntaxTree(QTextStream &out, bool isFormat) const
{
    out << _lineNum << " REM  " ;
    printFormatRunTime(out,_runTime.count,isFormat) << endl;
    out << "\t" << _comment << endl;
}

LetStatement::LetStatement(Context *context, int lineNum, const QString &source, const QStringList &argList)
    :Statement(context , lineNum,"LET",source)
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
            throw Exception(WrongAssignSyntax);
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

int LetStatement::exec()
{
    if(_leftExpr->getType() != exp_var){
        //左值不是变量
        throw Exception(WrongLeftValue);
    }
    //先解析,解析无误后再赋值
    int32_t parseRes = _rightExpr->value(_context);
    if(!_context->useCount.contains(_leftExpr->getRootData())){
        //新定义变量
        _context->useCount[_leftExpr->getRootData()] = 0;
    }
    _context->varTable[_leftExpr->getRootData()] = parseRes;
    _context->PC += 1;
    updateRunTime();
    return 1;
}

void LetStatement::printSyntaxTree(QTextStream &out , bool isFormat) const
{
    if(_buildException.type != NoException){
        Statement::printErrSyntaxTree(out , isFormat);
        return ;
    }
    out << _lineNum << " LET =  ";
    printFormatRunTime(out,_runTime.count,isFormat) << endl;

    if(_leftExpr->getType() == exp_var){
        out << "\t" << _leftExpr->getRootData() << " " ;
        //输出使用次数
        if(!_context->useCount.contains(_leftExpr->getRootData())){
            //代码未运行时，varTable 和 useCount中可能不存在该变量
            //前端一定只可能在run之后调用analyze命令
            printFormatRunTime(out , 0 , isFormat) << endl;
        }
        else{
            printFormatRunTime(out , _context->useCount[_leftExpr->getRootData()] , isFormat) << endl;
        }
    }
    else{
        _leftExpr->printExpTree(out , 1);
    }
    _rightExpr->printExpTree(out,1);
}

PrintStatement::PrintStatement(Context *context, int lineNum, const QString &source, const QStringList &argList)
    :Statement(context , lineNum,"PRINT",source)
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

int PrintStatement::exec()
{
    std::cout << _expr->value(_context) << std::endl;
    _context->PC += 1;
    updateRunTime();
    return 1;
}

void PrintStatement::printSyntaxTree(QTextStream &out , bool isFormat) const
{
    if(_buildException.type != NoException){
        Statement::printErrSyntaxTree(out , isFormat);
        return ;
    }
    out << _lineNum << " PRINT  ";
    printFormatRunTime(out , _runTime.count , isFormat) << endl;

    _expr->printExpTree(out,1);
}

InputStatement::InputStatement(Context *context, int lineNum, const QString &source, const QStringList &argList)
    :Statement(context , lineNum,"INPUT",source)
{
    try{
        _expr = new Expression(Expression::infix2Suffix(argList.join("")));
        if(_expr->getType() != exp_var){
            //检查左值合法性
            throw Exception(WrongLeftValue);
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

int InputStatement::exec()
{
    //assert _expr.type == exp_var
    std::cout << "?";
    int32_t input;
    //TODO 前端保证，用户只能输入一个整数
    std::cin >> input;
    std::cin.get();
    if(!_context->varTable.contains(_expr->getRootData())){
        //新定义变量
        _context->useCount[_expr->getRootData()] = 0;
    }
    _context->varTable[_expr->getRootData()] = input;
    _context->PC += 1;
    updateRunTime();
    return 1;
}

void InputStatement::printSyntaxTree(QTextStream &out , bool isFormat) const
{
    if(_buildException.type != NoException){
        Statement::printErrSyntaxTree(out , isFormat);
        return ;
    }
    out << _lineNum << " INPUT  ";
    printFormatRunTime(out , _runTime.count , isFormat) << endl;
    _expr->printExpTree(out,1);
}
IfStatement::IfStatement(Context *context, int lineNum, const QString& source, QStringList argList)
    :Statement(context , lineNum,"IF",source)
{
    try{
        //assert argList.size() >= 2
        if(argList.size() < 2){
            throw Exception(WrongIfSyntax);
        }
        if(*(argList.end() - 2) != "THEN"){
            //找不到合法的THEN子句
            throw Exception(WrongIfSyntax,"未找到THEN子句");
        }

        bool isNum = false;
        _destination = argList.last().toInt(&isNum);
        if(!isNum){
            throw Exception(WrongGotoDst,argList.last());
        }
        if(_destination < 0 || _destination > 1000000){
            throw Exception(WrongGotoDst,argList.last());
        }
        //去除THEN 和 destination
        argList.removeLast();
        argList.removeLast();
        if(argList.empty()){
            //空条件
            throw Exception(EmptyExpr,"IF子句条件为空");
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
            throw Exception(WrongCmpSyntax,"找不到比较运算符(>,=,<)");
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

int IfStatement::exec()
{
    //解析布尔表达式
    bool parseRes;
    if(_conditionOp == '>'){
        parseRes = _conditionLeft->value(_context) > _conditionRight->value(_context);
    }
    else if(_conditionOp == '='){
        parseRes = _conditionLeft->value(_context) == _conditionRight->value(_context);
    }
    else if(_conditionOp == '<'){
        parseRes = _conditionLeft->value(_context) < _conditionRight->value(_context);
    }

    //条件跳转
    if(parseRes){
        _context->gotoLine(_destination);
    }
    else{
        _context->PC += 1;
    }
    updateRunTime(parseRes);
    return 1;
}

void IfStatement::printSyntaxTree(QTextStream &out , bool isFormat) const
{
    if(_buildException.type != NoException){
        Statement::printErrSyntaxTree(out , isFormat);
        return ;
    }
    out << _lineNum << " IF THEN  ";
    printFormatRunTime(out , _runTime.conditionCount.trueCount , isFormat) << " ";
    printFormatRunTime(out , _runTime.conditionCount.falseCount , isFormat) << endl;
    _conditionLeft->printExpTree(out,1);
    out << '\t' << _conditionOp << endl;
    _conditionRight->printExpTree(out,1);
    out << '\t' << _destination << endl;
}

void IfStatement::updateRunTime(bool flag)
{
    if(flag){
        ++ _runTime.conditionCount.trueCount;
    }
    else{
        ++ _runTime.conditionCount.falseCount;
    }
}

void IfStatement::printRunTime() const
{
    std::cout << _runTime.conditionCount.trueCount << "\t"
              << _runTime.conditionCount.falseCount << std::endl;
}
GotoStatement::GotoStatement(Context *context, int lineNum, const QString &source, const QStringList &argList)
    :Statement(context , lineNum,"GOTO",source)
{
    bool isNum = false;
    //assert argList.size == 1
    try{
        if(argList.size() != 1){
            throw Exception(WrongGotoDst," ");
        }
        _destination = argList.first().toInt(&isNum);
        if(!isNum){
            throw Exception(WrongGotoDst,argList.first());
        }
        if(_destination < 0 || _destination > 1000000){
            throw Exception(WrongGotoDst,argList.first());
        }
    }
    catch(Exception e){
        _buildException = e;
    }
}

int GotoStatement::exec()
{
    _context->gotoLine(_destination);
    updateRunTime();
    return 1;
}

void GotoStatement::printSyntaxTree(QTextStream& out , bool isFormat) const
{
    if(_buildException.type != NoException){
        Statement::printErrSyntaxTree(out , isFormat);
        return ;
    }
    out << _lineNum << " GOTO  ";
    printFormatRunTime(out , _runTime.count , isFormat) << endl;
    out << '\t' << _destination << endl;
}

EndStatement::EndStatement(Context *context, int lineNum, const QString &source)
    :Statement(context , lineNum,"END",source)
{
}

int EndStatement::exec()
{
    _context->PC += 1;
    updateRunTime();
    return 0;
}

void EndStatement::printSyntaxTree(QTextStream &out , bool isFormat) const
{
    out << _lineNum << " END  ";
    printFormatRunTime(out , _runTime.count , isFormat) << endl;
}

ErrStatement::ErrStatement(Context* context ,int lineNum, const QString &source, Exception buildException)
    :Statement(context , lineNum,"ERR",source,buildException)
{
}

int ErrStatement::exec()
{
    //永远不会被调用
    //no need to update run time
    return 0;
}

void ErrStatement::printSyntaxTree(QTextStream& out, bool isFormat) const
{
    Statement::printErrSyntaxTree(out , isFormat);
}
