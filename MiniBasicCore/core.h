#ifndef CORE_H
#define CORE_H
#include "inc.h"
using namespace std;
class Core
{
public:
    Core();
    ~Core();
    int exec(int argc, char *argv[]);
public:
    void usage() const;
private:
    Context* context = nullptr;
    QFile treeDisplay;
    QTextStream treeOut;
};


#endif // CORE_H
