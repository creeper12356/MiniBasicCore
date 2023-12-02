#include "core.h"
#include <fstream>
#include <cstring>
using namespace std;
Core::Core()
{

}
int Core::exec(int argc,char* argv[])
{
    //0 stdin, 1 file
    int input_mode = 0;
    //初始化
    string input = "";
    ifstream fin;
    //输入流
    istream* in = nullptr;

    if(argc == 1){
        input_mode = 0;
        in = &cin;
    }
    else if(argc == 3 && strcmp(argv[1],"-i") == 0){
        input_mode = 1;
        fin.open(argv[2]);
        if(!fin){
            cerr << argv[2] << " does not exists.\n";
            return -1;
        }
        in = &fin;
    }
    else {
        cerr << "Wrong arguments.Abort.\n";
        return -1;
    }

    while(true){
        //只有标准输入模式打印终端提示符
        if(input_mode == 0){
            cout << ">>>";
        }
        if(!std::getline(*in,input)){
            break;
        }
        try {
            Statement statement = Statement(QString::fromStdString(input));
            if(statement.type() == "CMD"){
                //调试使用的命令
                if(statement.object() == "EXIT"){
                    cout << "logout" << endl;
                    return 0;
                }
                else if(statement.object() == "VARTABLE"){
                    printVarTable();
                }
                else if(statement.object() == "CODEHISTORY"){
                    printCodeHistory();
                }
                continue;
            }
            if(statement.type() == "REM"){
                //注释
                continue;
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
                    throw ParseError;
                }
                string left = statement.object().left(assignmentIndex).toStdString();
                string right = statement.object().right(statement.object().size() - assignmentIndex - 1).toStdString();
                //先保证解析正确再赋值
                int parseRes = parseExpression(right);
                varTable[left] = parseRes;
            }
            else if(statement.type() == "PRINT"){
                cout << parseExpression(statement.object().toStdString()) << endl;
            }
            else if(statement.type() == "INPUT"){
                //TODO 检查destination合法性
//                for(auto ch: statement.object()){

//                }
                cout << "?";
                int32_t input;
                //TODO 前端保证，用户只能输入一个整数
                cin >> input;
                if(!cin.good()){
                    //输入异常
                    cin.clear();
                    cin.get();
                    throw ParseError;
                }
                cin.get();
                varTable[statement.object().toStdString()] = input;
            }
            else if(statement.type() == "END"){
                //结束
                return 0;
            }

            else{
                //未知关键字
                cerr << "UnknownStatementType " << statement.type().toStdString() << "." << endl;
                throw UnknownStatementType;
            }
            //执行成功的代码
            codeHistory.append(statement.source());
        }
        catch(Exception e){
            cerr << "throw Exception " << int(e) << endl;
            continue;
        }
    }
    if(input_mode == 1){
        fin.close();
    }
    return 0;
}

ostream& Core::Infix2Suffix(ostream& os,const string &str)
{
//    TODO: 非法中缀表达式
    QStack<char> st;
    
    read_mode mode = read_other;
    int32_t digit = 0;
    string var = "";

    for(auto ch:str){
        if(isalpha(ch)){
            if(mode == read_var){
                //already reading var
                var.push_back(ch);
            }
            else if(mode == read_digit){
                //使用非法变量名
                cerr << "directly switch from alpha to digit.\n";
                throw ParseError;
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
                cout << "finish reading digit: " << digit << endl;
                os << digit << "]";
                mode = read_other;
                digit = 0;
            }
            else if(mode == read_var){
                //ready to finish reading var
                cout << "finish reading var: " << var << endl; 
                os << var << ")";
                mode = read_other;
                var.clear();
            }

            if(ch == '('){
                st.push(ch);
            }
            else if(ch == ')'){
                while(!st.empty() && st.top() != '('){
                    os << st.pop();
                }
                if(st.empty()){
                    //多余右括号不匹配
                    cerr << "blankets not match.\n";
                    throw ParseError;
                }
                //弹出'('
                st.pop();
            }
            else if(ch == '+' || ch == '-'){
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
                cerr << "unknown operator " << ch << endl;
                throw ParseError;
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
            cerr << "blankets not match.\n";
            throw ParseError;
        }
        os << st.pop();
    }
    return os;
}

int32_t Core::parseExpression(const string &str)
{
    //先处理纯数字的表达式
    //将中缀表达式转后缀
    ostringstream ss;
    Infix2Suffix(ss,str);
    string suffix(ss.str());
//    cout << "suffix: " << suffix << endl;

    //TODO: 假设变量没有下划线
    //1: reading digit , 2: reading variable, 0 other cases
    read_mode mode = read_other;
    int32_t digit = 0;
    string var = "";
    QStack<int32_t> st;
    int32_t op1,op2;//操作数
    for(auto ch: suffix){
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
        if(mode == read_var){
            var.push_back(ch);
            continue;
        }
        if(mode == read_digit){
            digit = digit * 10 + (ch - '0') ;
            continue;
        }
        //当前都为二元运算
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
        //TODO empty num
        cerr << "Empty expression.\n";
        throw ParseError;
    }
    return st.top();
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

void Core::printCodeHistory() const
{
    cout << "---CODEHISTORY---" << endl;
    for(auto code: codeHistory){
        cout << code.toStdString() << endl;
    }
    cout << "---\tEND\t---" << endl;
}
