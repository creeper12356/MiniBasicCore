#include "core.h"
#include <fstream>
#include <cstring>
using namespace std;
Core::Core()
    :operators({
               new OpPos,
               new OpNeg,
               new OpAdd,
               new OpMinus,
               new OpMulti,
               new OpDiv,
               new OpMod
               })
{
    PC = 0;
}

Core::~Core()
{
    for(auto op: operators){
        delete op;
    }
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
                        if(!exeCode(codes[PC].toStdString())){
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

int Core::exeCode(const string &code)
{
    bool isPCModified = false;
    Statement statement = Statement(QString::fromStdString(code));
    if(statement.type() == "CMD"){
        //调试使用的命令
        if(statement.object() == "VARTABLE"){
            printVarTable();
        }
    }
    else if(statement.type() == "REM"){
        //do nothing
    }
    else if(statement.type() == "LET"){
        int assignmentIndex = -1;
        //查找赋值号位置
        for(int i = 0;i < statement.object().size();++i){
            if(statement.object()[i] == '='){
                assignmentIndex = i;
                break;
            }
        }
        if(assignmentIndex == -1){
            //没有赋值号
            throw WrongAssignSyntax;
        }
        string left = statement.object().left(assignmentIndex).toStdString();
        string right = statement.object().right(statement.object().size() - assignmentIndex - 1).toStdString();
        //先保证解析正确再赋值
        int parseRes = parseInfixExpr(right);
        varTable[left] = parseRes;
    }
    else if(statement.type() == "PRINT"){
        cout << parseInfixExpr(statement.object().toStdString()) << endl;
    }
    else if(statement.type() == "INPUT"){
        //TODO 检查destination合法性
//                for(auto ch: statement.object()){

//                }
        cout << "?";
        int32_t input;
        //TODO 前端保证，用户只能输入一个整数
        cin >> input;
//                if(!cin.good()){
//                    输入异常
//                    cin.clear();
//                    cin.get();
//                    throw ParseError;
//                }
        cin.get();
        varTable[statement.object().toStdString()] = input;
    }
    else if(statement.type() == "GOTO"){
        gotoLine(statement.object().toInt());
        isPCModified = true;
    }
    else if(statement.type() == "IF"){
        IfStatement ifStatement(statement);
        if(parseBoolExpr(ifStatement.condition().toStdString())){
            gotoLine(ifStatement.destination());
            isPCModified = true;
        }
    }
    else if(statement.type() == "END"){
        //结束
        return 0;
    }

    else{
        //未知关键字
        throw UnknownStatementType;
    }
    if(!isPCModified)
    {
        //默认顺序执行
        ++PC;
    }
    return 1;
}

ostream& Core::infix2Suffix(ostream& os,const string &str)
{
    QStack<char> st;

    read_mode mode = read_other;
    int32_t digit = 0;
    string var = "";
    char ch;
    for(auto iterator = str.begin();iterator != str.end();++iterator){
        ch = *iterator;
        if(isalpha(ch)){
            if(mode == read_var){
                //already reading var
                var.push_back(ch);
            }
            else if(mode == read_digit){
                //使用非法变量名
//                cerr << "directly switch from alpha to digit.\n";
                throw WrongVarName;
            }
            else{
                //start to read var
                os << "(";
                mode = read_var;
                var.push_back(ch);
            }
        }
        else if(isdigit(ch)){
            if(mode == read_var){
                //already reading var
                var.push_back(ch);
            }
            else if(mode == read_digit){
                //already reading digit
                digit = digit * 10 + (ch - '0');
            }
            else{
                //start to read digit
                mode = read_digit;
                os << "[";
                digit = digit * 10 + (ch - '0');
            }
        }
        else{
            if(mode == read_digit){
                //ready to finish reading digit
//                cout << "finish reading digit: " << digit << endl;
                os << digit << "]";
                mode = read_other;
                digit = 0;
            }
            else if(mode == read_var){
                //ready to finish reading var
//                cout << "finish reading var: " << var << endl;
                os << var << ")";
                mode = read_other;
                var.clear();
            }

            if(ch == '('){
                st.push(ch);
                continue;
            }
            else if(ch == ')'){
                if(st.empty()){
                    //多余右括号不匹配
                    throw BracketsNotMatch;
                }
                if(iterator != str.begin() && *(iterator - 1) == '('){
                    //空括号
                    throw EmptyExpr;
                }
                while(st.top() != '('){
                    os << st.pop();
                }

                //弹出'('
                st.pop();
                continue;
            }
            if(ch == '+' || ch == '-'){
                if(iterator == str.begin() || *(iterator - 1) == '('){
                    //遇到正负号，补全操作数0
                    os << "[0]";
                }
                while(!st.empty() && (st.top() == '*' || st.top() == '/' || st.top() == '%' || st.top() == '+' || st.top() == '-')){
                    os << st.pop();
                }
                st.push(ch);
            }
            else if(ch == '*' || ch == '/' || ch == '%'){
                while(!st.empty() && (st.top() == '*' || st.top() == '/' || st.top() == '%')){
                    os << st.pop();
                }
                st.push(ch);
            }
            else {
                //未知运算符
                throw UnknownOp;
            }
        }
    }
    //处理最后一个变量
    if(mode == read_var){
        os << var << ")";
    }
    else if(mode == read_digit){
        os << digit << "]";
    }
    //弹出剩余运算符
    while(!st.empty()){
        if(st.top() == '('){
            //多余左括号不匹配
            throw BracketsNotMatch;
        }
        os << st.pop();
    }
    return os;
}

int32_t Core::parseSuffixExpr(const string &str)
{
    //TODO: 假设变量没有下划线
    //1: reading digit , 2: reading variable, 0 other cases
    read_mode mode = read_other;
    int32_t digit = 0;
    string var = "";
    QStack<int32_t> st;
    int32_t op1,op2;//操作数
    for(auto ch: str){
        //()和[]分别标记变量和常量
        //开始或结束读取变量或常量
        if(ch == '('){
            //start reading var
            mode = read_var;
            continue;
        }
        else if(ch == ')'){
            //finish reading var
            if(!varTable.contains(var)){
                throw UseBeforeDeclare;
            }
            st.push(varTable[var]);
            mode = read_other;
            var.clear();
            continue;
        }
        else if(ch == '['){
            //start reading digit
            mode = read_digit;
            continue;
        }
        else if(ch == ']'){
            //finish reading digit
            st.push(digit);
            mode = read_other;
            digit = 0;
            continue;
        }
        //正在读取变量或常量
        if(mode == read_var){
            var.push_back(ch);
            continue;
        }
        if(mode == read_digit){
            digit = digit * 10 + (ch - '0') ;
            continue;
        }
        //读入运算符
        //当前都为二元运算
        if(st.size() < 2){
            //运算符操作数不匹配
            throw ParseError;
        }
        op2 = st.pop();
        op1 = st.pop();
        //read other mode
        if(ch == '+'){
            st.push(op1 + op2);
        }
        else if(ch == '-'){
            st.push(op1 - op2);
        }
        else if(ch == '*'){
            st.push(op1 * op2);
        }
        else if(ch == '/'){
            st.push(op1 / op2);
        }
        else if(ch == '%'){
            st.push(op1 % op2);
        }
    }
    if(st.empty()){
        throw EmptyExpr;
    }
    if(st.size() > 1){
        throw ParseError;
    }
    //assert st.size() == 1
    return st.top();
}

int32_t Core::parseInfixExpr(const string &str)
{
    //先处理纯数字的表达式
    //将中缀表达式转后缀
    ostringstream ss;
    infix2Suffix(ss,str);
    string suffix(ss.str());
//    cout << "suffix: " << suffix << endl;
    return parseSuffixExpr(suffix);
}

bool Core::parseBoolExpr(const string &expr)
{
    auto it = expr.begin();
    for(;it != expr.end();++it){
        if(*it == '<' || *it == '=' || *it == '>'){
            break;
        }
    }
    if(it == expr.end()){
        //找不到<,=,>
        throw WrongCmpSyntax;
    }
    int32_t left = parseInfixExpr(expr.substr(0,it - expr.begin()));
    int32_t right = parseInfixExpr(expr.substr(it - expr.begin() + 1,expr.end() - 1 - it));

//    std::cout << "left: " << left << std::endl;
//    std::cout << "right: " << right << std::endl;
    //比较大小
    if(*it == '<'){
        return left < right;
    }
    if(*it == '='){
        return left == right;
    }
    if(*it == '>'){
        return left > right;
    }
    return false;
}

void Core::gotoLine(int dst)
{
    for(int i = 0;i <= codes.size();++i){
        //逐一比较行号
        if(dst == codes[i].split(" " , QString::SkipEmptyParts)[0].toInt()){
            PC = i;
            return ;
        }
    }
    //目标不存在
    //TODO : throw exception when dst not exist
    cerr << "destination not exist.\n";
    throw WrongGotoDst;
}

void Core::printVarTable() const
{
    cout << "---VARTABLE---" << endl;
    cout << "name\tvalue\n";
    cout << "----\t-----\n";
    for(auto name:varTable.keys()){
        cout << name << "\t" << varTable[name] << endl;
    }
    cout << "---\tEND\t---" << endl;
}
