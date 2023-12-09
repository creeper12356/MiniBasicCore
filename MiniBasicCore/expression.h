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
    //将中缀表达式转为后缀表达式
    static QString infix2Suffix(const QString& infix);
public:
    //通过后缀表达式构造表达式树
    Expression(const QString& suffix);
    ~Expression();

    ExpType getType() const {return type;}
    //返回根节点存储的字符串
    const QString& getRootData() const{return root->data;}
    //计算表达式树的值
    int32_t value(Core *context, ExpNode* node);
    int32_t value(Core* context);
};

#endif // EXPRESSION_H
