#include "expression.h"

Expression::Expression()
{
    root = new ExpNode("");
    type = "NULL";
}

Expression::~Expression()
{
    delete root;
}

ExpNode::ExpNode(const QString& d, ExpNode *l, ExpNode *r):
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

