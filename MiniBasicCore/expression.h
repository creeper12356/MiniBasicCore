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
//运算符，最多接收两个运算数
class Op{
protected:
    //对应的字符
    char sign;
    //优先级，从1开始
    int level;
public:
    Op(char sign,int level);
    virtual ~Op(){}
    //子类必须实现的运算函数
    virtual int32_t calc(int32_t n1,int32_t n2) = 0;
};
class OpPos: public Op{
public:
    OpPos():Op('+',-1){}
    int32_t calc(int32_t n1, int32_t n2) override{return n1;}
};
class OpNeg: public Op{
public:
    OpNeg():Op('-',-1){}
    int32_t calc(int32_t n1, int32_t n2) override{return -n1;}
};

class OpAdd: public Op{
public:
    OpAdd():Op('+',1){}
    int32_t calc(int32_t n1, int32_t n2) override{return n1 + n2;}
};
class OpMinus: public Op{
public:
    OpMinus():Op('-',1){}
    int32_t calc(int32_t n1, int32_t n2) override{return n1 - n2;}
};
class OpMulti: public Op{
public:
    OpMulti():Op('*',2){}
    int32_t calc(int32_t n1, int32_t n2) override{return n1 * n2;}
};
class OpDiv: public Op{
public:
    OpDiv():Op('/',2){}
    int32_t calc(int32_t n1, int32_t n2) override{return n1 / n2;}
};
class OpMod: public Op{
public:
    OpMod():Op('%',2){}
    int32_t calc(int32_t n1, int32_t n2) override{return n1 % n2;}
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
