#include "CoreServer.hpp"
#include "CustomDefines.hpp"
#include <stdio.h>
#include <cstdlib>

int main()
{
    CoreServer core(4242);
    char *shell_path = STRDUP(SHELL_PATH);

    SETENV(shell_path);
    core.start();
    std::free(shell_path);
}