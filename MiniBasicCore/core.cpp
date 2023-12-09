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
    if(statement == nullptr){
        //TODO
        return 0;
    }
    int res = statement->exec(this);
    delete statement;
    return res;
}

QString Core::infix2Suffix(const QString &str)
{
    QStack<QChar> st;
    QString ret = "";

    read_mode mode = read_other;
    int32_t digit = 0;
    QString var = "";
    QChar ch;
    for(auto iterator = str.begin();iterator != str.end();++iterator){
        ch = *iterator;
        if(ch.isLetter()){
            if(mode == read_var){
                //already reading var
                var.push_back(ch);
            }
            else if(mode == read_digit){
                //使用非法变量名
                throw WrongVarName;
            }
            else{
                //start to read var
                ret.append("(");
                mode = read_var;
                var.push_back(ch);
            }
        }
        else if(ch.isDigit()){
            if(mode == read_var){
                //already reading var
                var.push_back(ch);
            }
            else if(mode == read_digit){
                //already reading digit
                digit = digit * 10 + (ch.unicode() - '0');
            }
            else{
                //start to read digit
                mode = read_digit;
                ret.append("[");
                digit = digit * 10 + (ch.unicode() - '0');
            }
        }
        else{
            if(mode == read_digit){
                //ready to finish reading digit
                ret.append(QString::number(digit) + "]");
                mode = read_other;
                digit = 0;
            }
            else if(mode == read_var){
                //ready to finish reading var
                ret.append(var + ")");
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
                    ret.append(st.pop());
                }

                //弹出'('
                st.pop();
                continue;
            }
            if(ch == '+' || ch == '-'){
                if(iterator == str.begin() || *(iterator - 1) == '('){
                    //遇到正负号，补全操作数0
                    ret.append("[0]");
                }
                while(!st.empty() && (st.top() == '*' || st.top() == '/' || st.top() == '%' || st.top() == '+' || st.top() == '-')){
                    ret.append(st.pop());
                }
                st.push(ch);
            }
            else if(ch == '*' || ch == '/' || ch == '%'){
                while(!st.empty() && (st.top() == '*' || st.top() == '/' || st.top() == '%')){
                    ret.append(st.pop());
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
        ret.append(var + ")");
    }
    else if(mode == read_digit){
        ret.append(QString::number(digit) + "]");
    }
    //弹出剩余运算符
    while(!st.empty()){
        if(st.top() == '('){
            //多余左括号不匹配
            throw BracketsNotMatch;
        }
        ret.append(st.pop());
    }
    return ret;
}

int32_t Core::parseSuffixExpr(const QString &str)
{
    //TODO: 假设变量没有下划线
    //1: reading digit , 2: reading variable, 0 other cases
    read_mode mode = read_other;
    int32_t digit = 0;
    QString var = "";
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
            digit = digit * 10 + (ch.unicode() - '0') ;
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

int32_t Core::parseInfixExpr(const QString &str)
{
    QString suffix = infix2Suffix(str);
    return parseSuffixExpr(suffix);
}

bool Core::parseBoolExpr(const QString &expr)
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
    int32_t left = parseInfixExpr(expr.left(it - expr.begin()));
    int32_t right = parseInfixExpr(expr.right(expr.end() - 1 - it));

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
