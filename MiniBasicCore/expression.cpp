#include "expression.h"
#include "core.h"
QString Expression::infix2Suffix(const QString &str)
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
Expression::Expression(const QString &str)
{
    read_mode mode = read_other;
    QString digit = "";
    QString var = "";
    QStack<ExpNode*> st;
    ExpNode *leftNode = nullptr,
            *rightNode = nullptr;
    try{
        for(auto ch: str){
            //()和[]分别标记变量和常量
            //开始或结束读取变量或常量
            if(ch == '('){
                //开始读取变量
                mode = read_var;
                continue;
            }
            else if(ch == ')'){
                //完成读取变量
                st.push(new ExpNode(node_var,var));
                mode = read_other;
                var.clear();
                continue;
            }
            else if(ch == '['){
                //开始读取数字
                mode = read_digit;
                continue;
            }
            else if(ch == ']'){
                //完成读取数字
                st.push(new ExpNode(node_digit,digit));
                mode = read_other;
                digit.clear();
                continue;
            }
            if(mode == read_var){
                //正在读取变量
                var.push_back(ch);
                continue;
            }
            if(mode == read_digit){
                //正在读取数字
                digit.push_back(ch);
                continue;
            }

            //读取运算符
            if(st.size() < 2){
                //运算数小于2
                //运算符操作数不匹配
                throw ParseError;
            }
            rightNode = st.pop();
            leftNode = st.pop();
            st.push(new ExpNode(node_op,QString(ch),leftNode,rightNode));
        }
        if(st.empty()){
            throw EmptyExpr;
        }
        if(st.size() > 1){
            throw ParseError;
        }
    }
    catch(Exception e){
        //处理已分配动态空间
        for(auto node:st){
            delete node;
        }
        if(leftNode) delete leftNode;
        if(rightNode) delete rightNode;
        //向上一层抛出异常
        throw e;
    }

    //assert st.size() == 1
    root = st.top();

    //判断表达式种类
    if(root->type == node_var){
        this->type = exp_var;
    }
    else if(root->type == node_digit){
        this->type = exp_digit;
    }
    else if(root->type == node_op){
        this->type = exp_compound;
    }
}

Expression::~Expression()
{
    if(root){
        delete root;
    }
}

int32_t Expression::value(Core *context, ExpNode *node)
{
    //assert node != nullptr
    if(node->type == node_var){
        if(!context->varTable.contains(node->data)){
            //使用未定义的变量
            throw UseBeforeDeclare;
        }
        else{
            return context->varTable[node->data];
        }
    }
    if(node->type == node_digit){
        //assert node->data.isInt()
        return node->data.toInt();
    }

    //node->type == node_op
    if(node->data == "+"){
        return value(context, node->left) + value(context, node->right);
    }
    else if(node->data == "-"){
        return value(context, node->left) - value(context, node->right);
    }
    else if(node->data == "*"){
        return value(context, node->left) * value(context, node->right);
    }
    else if(node->data == "/"){
        return value(context, node->left) / value(context, node->right);
    }
    else if(node->data == "%"){
        return value(context, node->left) % value(context, node->right);
    }
}

int32_t Expression::value(Core *context)
{
    //assert root != nullptr
    return value(context,root);
}

ExpNode::ExpNode(ExpNodeType t, const QString& d, ExpNode *l, ExpNode *r):
    type(t),
    data(d),
    left(l),
    right(r)
{

}

ExpNode::~ExpNode()
{
    if(left){
        delete left;
    }
    if(right){
        delete right;
    }
}

