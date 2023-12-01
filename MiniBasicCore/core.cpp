#include "core.h"
using namespace std;
Core::Core()
{

}
int Core::exec()
{
    //初始化
    string input = "";

    while(true){
        //终端符号
        cout << ">>>";
        getline(cin,input);
        if(input == "exit"){
            cout << "logout" << endl;
            return 0;
        }
        try {
            Statement statement = Statement(QString::fromStdString(input));
//            cout << "------" << endl;
//            cout << "lineNum: " << statement.lineNum() << endl;
//            cout << "type: " << statement.type().toStdString() << endl;
//            cout << "object: " << statement.object().toStdString() << endl;
//            cout << "------" << endl;
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

                varTable[left] = parseExpression(right);
            }
            else if(statement.type() == "PRINT"){
                cout << "object: " << statement.object().toStdString() << endl;
                cout << parseExpression(statement.object().toStdString()) << endl;
            }
            else if(statement.type() == "INPUT"){
                //TODO check destination
//                for(auto ch: statement.object()){

//                }
                cout << "?";
                int32_t input;
                cin >> input;
                cin.get();
                varTable[statement.object().toStdString()] = input;
            }
            else if(statement.type() == "END"){
                return 0;
            }
        }
        catch(Error){
            cout << "error." << endl;
            continue;
        }
//        cout << input << endl;
//        cout << endl;
    }
    return 0;
}

ostream& Core::Infix2Suffix(ostream& os,const string &str)
{
    //TODO: 非法中缀表达式
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
                cerr << "directly switch from alpha to digit.";
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
        return -1;
    }
    return st.top();
}
