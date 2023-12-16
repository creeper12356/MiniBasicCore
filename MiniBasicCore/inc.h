#ifndef INC_H
#define INC_H
#include <QProcess>
#include <QDebug>
#include <QVector>
#include <QQueue>
#include <QStack>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QDir>
#include <QDataStream>
#include <QByteArray>

#include <iostream>
#include <string>

#include "exception.h"
#define qStdout QTextStream(stdout)
#define qStdin QTextStream(stdin)

typedef QString StatementType;

enum read_mode{read_other = 0,read_digit,read_var};

class Core;
class Statement;
class Context;
#endif // INC_H
