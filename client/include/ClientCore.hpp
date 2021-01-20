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
        void handle_input(const std::string &input);
        void analyse_messages_from_TCP(void);
        void send_command_to_TCP_server(const std::string &cmd);
        void ask_informations();
        void new_prompt() const;

        std::string _host;
        std::string _port;
        std::string _pwd;
        std::string _os;
        bool _running;
        FPSManager _fpsManager;
        Network _network;
};

#endif /* !CLIENTCORE_HPP_ */
