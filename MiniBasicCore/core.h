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
};


#endif // CORE_H
