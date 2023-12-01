#ifndef EXPRESSION_H
#define EXPRESSION_H
#include <QString>
//变量
class Var{
public:
    QString name;
    int32_t value;
};
//运算符
//class Op{
//public:
//    QString content;
//    //优先级，从1开始
//    int level;

//    Op(const QString& c,int l):content(c),level(l){}
//    Op(){}
//};

typedef QString ExpressionType;
//表达式树节点
struct ExpNode{
    QString data;
    ExpNode* left;
    ExpNode* right;

    ExpNode(const QString& data,ExpNode* left = nullptr,ExpNode* right = nullptr);
    ~ExpNode();
};

class Expression
{
    ExpressionType type;
    ExpNode* root;
public:
    Expression();
    ~Expression();
};

#endif // EXPRESSION_H
