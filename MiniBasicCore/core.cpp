#include "core.h"
#include "statement.h"
#include "expression.h"

Core::Core()
{
    PC = 0;
}

Core::~Core()
{

}
int Core::exec(int argc,char* argv[])
{
    bool silentFlag = false;
    if(argc == 2 && strcmp(argv[1],"-s") == 0){
        //-s选项，不进行任何调试输出
        silentFlag = true;
    }
    string cmd;
    while(true){
        (!silentFlag) && cout << ">>>";
        if(!getline(cin,cmd)){
            break;
        }
        if(cmd == "exit"){
            break;
        }
        else if(cmd == "clear"){
            QProcess::execute("clear");
        }
        else if(cmd == "load"){
            //从标准输入加载代码
            //清空原来代码
            codes.clear();
            string code;
            while(true){
                (!silentFlag) && cout << "$";
                if(!getline(cin,code)){
                    //TODO exception when typing ctrlD
                    cin.clear();
                    break;
                }

                //遇到空行停止接收输入
                if(code == ""){
                    break;
                }
                codes.append(QString::fromStdString(code));
            }
        }
        else if(cmd == "append"){
            string code;
            while(true){
                (!silentFlag) && cout << "$";
                getline(cin,code);
                if(code == ""){
                    //空行停止接收输入
                    break;
                }
                codes.append(QString::fromStdString(code));
            }
        }
        else if(cmd == "list"){
            //列出加载的代码
            for(auto code:codes){
                cout << code.toStdString() << endl;
            }
        }
        else if(cmd == "run"){
            //重置PC
            PC = 0;
            //清空变量表
            varTable.clear();
            while(PC != codes.size()){
                try {
                        if(!exeCode(codes[PC])){
                            //退出
                            break;
                        }
                    }
                catch(Exception e){
                    cerr << "throw Exception " << int(e) << endl;
                    ++PC;
                    continue;
                }
            }
            (!silentFlag) && cout << "run finished.\n";
        }

    }
    cout << "logout\n";
    return 0;
}

int Core::exeCode(const QString &code)
{
    Statement* statement = Statement::newStatement(code);
    if(statement->getBuildException() != NoException){
        //构造失败
        Exception buildException = statement->getBuildException();
        delete statement;
        throw buildException;
    }
    if(statement == nullptr){
        //TODO
        return 0;
    }
    int res = statement->exec(this);
    delete statement;
    return res;
}

void Core::gotoLine(int dst)
{
    for(int i = 0;i < codes.size();++i){
        //逐一比较行号
        if(dst == codes[i].split(" " , QString::SkipEmptyParts)[0].toInt()){
            PC = i;
            return ;
        }
    }
    //目标不存在
    throw WrongGotoDst;
}

void Core::printVarTable() const
{
//    cout << "---VARTABLE---" << endl;
//    cout << "name\tvalue\n";
//    cout << "----\t-----\n";
//    for(auto name:varTable.keys()){
//        cout << name << "\t" << varTable[name] << endl;
//    }
//    cout << "---\tEND\t---" << endl;
}
