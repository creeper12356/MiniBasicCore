#include <iostream>
#include <string>

#include "statement.h"
#include "core.h"
using namespace std;

int main(int argc, char *argv[])
{
    Core core;
    return core.exec(argc,argv);
}
