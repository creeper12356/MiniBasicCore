#include "expression.h"
Expression::Expression()
{
//    root = new ExpNode("");
//    type = "NULL";
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
        this->type == exp_var;
    }
    else if(root->type == node_digit){
        this->type == exp_digit;
    }
    else if(root->type == node_op){
        this->type == exp_compound;
    }
}

Expression::~Expression()
{
    if(root){
        delete root;
    }
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

