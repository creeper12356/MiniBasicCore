#ifndef EXPRESSION_H
#define EXPRESSION_H
#include <QString>
//变量
class Var{
public:
    QString name;
    int32_t value;
};

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
