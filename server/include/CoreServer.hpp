#ifndef CORESERVER_HPP_
#define CORESERVER_HPP_

#include "NetworkServer.hpp"
#include "FPSManager.hpp"

class CoreServer {
    public:
        CoreServer(int port);
        ~CoreServer() = default;
        void start();

    protected:
    private:
        void analyse_messages();
        const std::string execute_shell_command(const char *cmd);
        void send_informations_to_client(FWNetwork::OwnedMessageTCP<RemoteShell::TCPCustomMessageID> &msgGet);
        const std::string get_windows_os();
        const std::string get_windows_pwd();
        const std::string get_other_os();
        const std::string get_other_pwd();
        NetworkServer _network;
        FPSManager _fpsManager;
        bool _running;
};

#endif /* !CORESERVER_HPP_ */
