#ifndef CORE_H
#define CORE_H
#include "inc.h"
using namespace std;
/* 核心程序 */
class Core
{
public:
    Core();
    ~Core();
    int exec(int argc, char *argv[]);
public:
    /* 帮助 */
    void usage() const;
private:
    Context* context = nullptr;
};


#endif // CORE_H
