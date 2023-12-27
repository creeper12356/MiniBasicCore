#include "context.h"
#include "statement.h"
#include "expression.h"
using namespace std;
Context::Context()
{
    
}

Context::~Context()
{
    for(auto code: codes){
        delete code;
    }   
}

void Context::appendCode(const QString &code)
{
    codes.append(Statement::newStatement(this,code));
}

void Context::clearRunningStatus()
{
    varTable.clear();
    useCount.clear();
    PC = 0;
    for(Statement* code: codes){
        code->clearRunTime();
    }
}

void Context::listCodes() const
{
    for(Statement* code: codes){
        cout << code->getSource().toStdString() << endl;
    }
}

void Context::analyze(QTextStream &out) const
{
    for(Statement* code: codes){
        code->printSyntaxTree(out , true);
    }
}

void Context::printVarTable() const
{
    for(auto var: useCount.keys()){
        std::cout << var.toStdString() << "\t" << varTable[var] << std::endl;
    }
}

void Context::printRunTime() const
{
    for(Statement* code: codes){
        code->printRunTime();
    }
}

void Context::printUseCount() const
{
    for(auto var: useCount.keys()){
        std::cout << var.toStdString() << "\t" << useCount[var] << std::endl;
    }
}

int Context::executeCode(Statement *code)
{
    try {
        if(code->getBuildException().type != NoException){
            throw code->getBuildException();
        }
        else{
            return code->exec();
        }
    }
    catch(Exception e){
        cerr << code->getLineNumStr().toStdString() << ": ";
        cerr << e.toStdString() << endl;
        ++PC;
        return 1;
    }
}

int Context::getPC() const
{
    return PC;
}

void Context::runCodes()
{
    while(PC != codes.size() && executeCode(codes[PC]));
}

void Context::clearCodes()
{
    for(auto code: codes){
        delete code;
    }
    codes.clear();
}

void Context::gotoLine(int dst)
{
    for(int i = 0;i < codes.size();++i){
        //逐一比较行号
        if(dst == codes[i]->getLineNum()){
            PC = i;
            return ;
        }
    }
    //目标不存在
    throw Exception(WrongGotoDst,QString::number(dst));
}
