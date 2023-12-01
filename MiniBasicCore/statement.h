#ifndef STATEMENT_H
#define STATEMENT_H
#include <string>
#include <iostream>
#include <QString>
#include <QStringList>

typedef QString StatementType;
enum Error{
    ParseError
};

class Statement{
public:
    Statement(const QString& src);
public:
    int lineNum() const{return _lineNum;}
    const StatementType& type() const{return _type;}
    const QString& source() const{return _source;}
    const QString& object() const{return _object;}
private:
    int _lineNum;
    StatementType _type;
    QString _source;
    QString _object;
};

#endif // STATEMENT_H
