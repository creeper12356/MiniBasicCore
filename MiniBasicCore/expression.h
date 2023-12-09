#ifndef EXPRESSION_H
#define EXPRESSION_H
#include "inc.h"
//变量
class Var{
public:
    QString name;
    int32_t value;
};
enum ExpNodeType{node_var,node_digit,node_op};
enum ExpType{exp_var,exp_digit,exp_compound};
//表达式树节点
struct ExpNode{
    ExpNodeType type;
    QString data;
    ExpNode* left;
    ExpNode* right;

    ExpNode(ExpNodeType type,const QString& data,ExpNode* left = nullptr,ExpNode* right = nullptr);
    ~ExpNode();
};

class Expression
{
    ExpType type;
    ExpNode* root = nullptr;
public:
    Expression();
    //通过后缀表达式构造表达式树
    Expression(const QString& suffix);
    ~Expression();
};

#endif // EXPRESSION_H
