#include "CoreServer.hpp"
#include "CustomDefines.hpp"
#include <stdio.h>

int main()
{
    CoreServer core(4242);

    SETENV(SHELL_PATH);
    core.start();
}