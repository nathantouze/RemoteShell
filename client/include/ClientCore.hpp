#ifndef CLIENTCORE_HPP_
#define CLIENTCORE_HPP_

#include "Network.hpp"
#include "FPSManager.hpp"
#include <string>

class ClientCore {
    public:
        ClientCore(const std::string &host, const std::string &port);
        ~ClientCore();
        void start();

    protected:
    private:
        void analyse_messages_from_TCP(void);
        void send_command_to_TCP_server(const std::string &cmd);

        std::string _host;
        std::string _port;
        bool _running;
        FPSManager _fpsManager;
        Network _network;
};

#endif /* !CLIENTCORE_HPP_ */