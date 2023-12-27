#include "core.h"
#include "context.h"
#include "statement.h"
Core::Core()
{
    context = new Context;
}

Core::~Core()
{
    delete context;
}
int Core::exec(int argc,char* argv[])
{
    bool backendFlag = false;
    if(argc == 2 && strcmp(argv[1],"-b") == 0){
        backendFlag = true;
    }
    string cmd;
    while(true){
        (!backendFlag) && (cout << ">>>");
        if(!getline(cin,cmd)){
            break;
        }
        QStringList argList = QString::fromStdString(cmd).split(" ",QString::SkipEmptyParts);
        if(argList.size() == 0){
            continue;
        }
        if(argList[0] == "exit"){
            break;
        }
        else if(argList[0] == "clear"){
            context->clearCodes();
            context->clearRunningStatus();
        }
        else if(argList[0] == "load"){
            //从标准输入加载代码
            context->clearCodes();
            string code;
            while(true){
                (!backendFlag) && (cout << "$");
                if(!getline(cin,code)){
                    //TODO exception when typing ctrl + D
                    cin.clear();
                    break;
                }

                //遇到空行停止接收输入
                if(code == ""){
                    break;
                }
                context->appendCode(QString::fromStdString(code));
            }
        }
        else if(argList[0] == "append"){
            string code;
            while(true){
                (!backendFlag) && (cout << "$");
                getline(cin,code);
                if(code == ""){
                    //空行停止接收输入
                    break;
                }
                context->appendCode(QString::fromStdString(code));
            }
        }
        else if(argList[0] == "list"){
            context->listCodes();
        }
        else if(argList[0] == "pc"){
            cout << "PC = " << context->getPC() << endl;
        }
        else if(argList[0] == "run"){
            context->clearRunningStatus();
            context->runCodes();
            //mark run finished
            backendFlag && (cout << "$");
        }
        else if(argList[0] == "analyze"){
            QTextStream out(stdout);

            backendFlag && (cout << "#");
            context->analyze(out , backendFlag);
            backendFlag && (cout << "#");
        }
        else if(argList[0] == "vartable"){
            context->printVarTable();
        }
        else if(argList[0] == "runtime"){
            context->printRunTime();
        }
        else if(argList[0] == "usecount"){
            context->printUseCount();
        }
        else if(argList[0] == "cmd"){
            QString qCmd = QString::fromStdString(cmd);
            qCmd.remove(0,strlen("cmd"));
            //使用行号0表示脚本
            Statement *stmt = Statement::newStatement(context , "0" + qCmd,false);
            context->executeCode(stmt);
            delete stmt;
        }
        else if(argList[0] == "help"){
            usage();
        }
    }
    cout << "logout\n";
    return 0;
}

void Core::usage() const
{
    cout << "MiniBasic Core Developed by Creeper\n"
            "---\t\t---\n";
    cout << "help:\t\tprint this message\n"
            "clear:\t\tclear context state\n"
            "load:\t\tload codes from stdin\n"
            "append:\t\tappend codes from stdin\n"
            "list:\t\tlist loaded codes\n"
            "analyze:\tprint syntax tree of loaded codes\n"
            "cmd:\t\tdirectly execute BASIC code\n"
            "exit:\t\tquit this program\n";
}

