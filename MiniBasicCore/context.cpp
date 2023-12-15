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
    codes.append(Statement::newStatement(code));
}

void Context::clearRunningStatus()
{
    varTable.clear();
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

void Context::analyze() const
{
    for(Statement* code: codes){
        code->printSyntaxTree();
    }
}

void Context::runTime() const
{
    for(Statement* code: codes){
        code->printRunTime();
    }
}

int Context::executeCode(Statement *code)
{
    try {
        if(code->getBuildException() != NoException){
            throw code->getBuildException();
        }
        else{
            return code->exec(this);
        }
    }
    catch(Exception e){
        cerr << "throw Exception " << int(e) << endl;
        ++PC;
        return 1;
    }
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
    throw WrongGotoDst;
}
