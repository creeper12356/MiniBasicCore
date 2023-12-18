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
//层序遍历使用的结构体
struct ExpNodeWrapper{
    ExpNode* node;
    int level;
    ExpNodeWrapper():node(nullptr),level(0){}
    ExpNodeWrapper(ExpNode* n,int l):node(n),level(l){}
};

class Expression
{
    ExpType type;
    ExpNode* root = nullptr;
public:
    //将中缀表达式转为后缀表达式
    static QString infix2Suffix(const QString& infix);
public:
    //TODO: 直接通过中缀表达式构造表达式树
    //通过后缀表达式构造表达式树
    Expression(const QString& suffix);
    ~Expression();

    ExpType getType() const {return type;}
    //返回根节点存储的字符串
    const QString& getRootData() const{return root->data;}
    //计算表达式树的值
    int32_t value(Context *context, ExpNode* node);
    int32_t value(Context *context);

    //在baseIndentation缩进下打印表达式树到流out
    void printExpTree(QTextStream& out,int baseIndentation = 0) const;
};

#endif // EXPRESSION_H
