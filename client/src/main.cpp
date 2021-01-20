#include "ClientCore.hpp"
#include <iostream>
#include <string>

int main(int ac, char **av)
{
    if (ac != 3) {
        std::cout << "./RemoteShell_client(.exe) [host] [port]" << std::endl << std::endl;
        std::cout << "\thost\tIP address of the host" << std::endl;
        std::cout << "\tport\tOpen port of the host" << std::endl;
        return (1);
    }
    std::string host = av[1];
    std::string port = av[2];
    ClientCore core(host, port);
    core.start();
    return (0);
}