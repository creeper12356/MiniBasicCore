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
            cout << "------" << endl;
            cout << "lineNum: " << statement.lineNum() << endl;
            cout << "type: " << statement.type().toStdString() << endl;
            cout << "object: " << statement.object().toStdString() << endl;
            cout << "------" << endl;
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
//                cout << "left: " << statement.object().left(assignmentIndex).toStdString() << endl;
                parseExpression(statement.object().right(statement.object().size() - assignmentIndex - 1).toStdString());
            }
        }
        catch(Error){
            cout << "error." << endl;
            continue;
        }
        cout << input << endl;
        cout << endl;
    }
    return 0;
}

ostream& Core::Infix2Suffix(ostream& os,const string &str)
{
    //TODO: 非法中缀表达式
    QStack<char> st;
    enum read_mode{read_other = 0,read_digit,read_var};
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
            else if(ch == '*' || ch == '/'){
                st.push(ch);
            }
            else if(ch == '+' || ch == '-'){
                while(!st.empty() && (st.top() == '*' || st.top() == '/' || st.top() == '+' || st.top() == '-')){
                    os << st.pop();
                }
                st.push(ch);
            }
            else if(ch == '*' || ch == '/'){
                while(!st.empty() && (st.top() == '*' || st.top() == '/')){
                    os << st.pop();
                }
                st.push(ch);
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
    cout << "suffix: " << suffix << endl;
    return 0;

    enum read_mode{read_other = 0,read_digit,read_var};
    read_mode mode = read_other;
    int32_t digit = 0;
    string var = "";
    //TODO: 假设变量没有下划线
    //1: reading digit , 2: reading variable, 0 other cases
    for(auto ch: suffix){
        if(isdigit(ch)){
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
                digit = digit * 10 + (ch - '0');
            }
        }
        else if(isalpha(ch)){
            if(mode == read_digit){
                //actually is not legal
                //switch to read var
                cerr << "directly switch from alpha to digit.";
                throw ParseError;
                // cout << "finish reading digit: " << digit << endl;
                // mode = 2;
                // digit = 0;
                // var.clear();
                // var.push_back(ch);
            }
            else if(mode == read_var){
                //already reading var
                var.push_back(ch);
            }
            else{
                //start to read var
                mode = read_var;
                var.push_back(ch);
            }
        }
        else{
            //标识符
            if(mode == read_digit){
                //ready to finish reading digit
                cout << "finish reading digit: " << digit << endl;
                mode = read_other;
                digit = 0;
            }
            else if(mode == read_var){
                //ready to finish reading var
                cout << "finish reading var: " << var << endl; 
                mode = read_other;
                var.clear();
            }
        }
    }
    return 0;
}
