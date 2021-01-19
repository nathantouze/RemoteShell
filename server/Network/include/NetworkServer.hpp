#ifndef NETWORKSERVER_HPP_
#define NETWORKSERVER_HPP_

#include "TCPServer.hpp"
#include "UDPServer.hpp"

class NetworkServer {
    public:
        NetworkServer(unsigned int tcp_port);
        ~NetworkServer();
        void startTCPServer();
        void stopTCPServer();
        void update();
        uint32_t startNewUDPServer();
        void stopUDPServer(uint32_t port);
        std::vector<std::shared_ptr<RemoteShell::UDPServer>> &getUDPServers();
        std::shared_ptr<RemoteShell::UDPServer> getUDPServer(uint32_t port);
        FWNetwork::ThreadQueue<FWNetwork::OwnedMessageTCP<RemoteShell::TCPCustomMessageID>> &getMessagesFromTCP();

    protected:
    private:
        RemoteShell::TCPServer _tcpServer;
        std::vector<std::shared_ptr<RemoteShell::UDPServer>> _udpServers;
};

#endif /* !NETWORKSERVER_HPP_ */
