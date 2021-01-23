#ifndef CORESERVER_HPP_
#define CORESERVER_HPP_

#include "NetworkServer.hpp"
#include "FPSManager.hpp"
#include "PWD.hpp"

class CoreServer {
    public:
        CoreServer(int port);
        ~CoreServer() = default;
        void start();

    protected:
    private:
        void analyse_messages();
        const std::string execute_shell_command(const char *cmd);
        void change_directory(FWNetwork::OwnedMessageTCP<RemoteShell::TCPCustomMessageID> &msgGet);
        void send_informations_to_client(FWNetwork::OwnedMessageTCP<RemoteShell::TCPCustomMessageID> &msgGet);
        const std::string get_os();
        const std::string get_username();
        const std::vector<std::string> split_string(const std::string string, char separator) const;
        void replaceAll(std::string &str, const std::string &before, const std::string &after);
        NetworkServer _network;
        FPSManager _fpsManager;
        bool _running;
        PWD _pwd;
};

#endif /* !CORESERVER_HPP_ */
