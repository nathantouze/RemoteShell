
#ifndef CLIENTNETWORK_HPP
#define CLIENTNETWORK_HPP

#include "AsioTCPClient.hpp"
#include "AsioUDPClient.hpp"
#include "CustomMessageIDs.hpp"

class Network {
    public:
        Network();
        void startTCPClient(std::string host, std::string port);
        void stopTCPClient();
        bool isConnectedToTCP() const;
        FWNetwork::ThreadQueue<FWNetwork::OwnedMessageTCP<RemoteShell::TCPCustomMessageID>>& getMessagesFromTCP(void);
        void sendMessagesToTCP(std::vector<FWNetwork::Message<RemoteShell::TCPCustomMessageID>>& messages);
        void startUDPClient(uint32_t port);
        void stopUDPClient();
        FWNetwork::Asio::UDP::ClientBase<RemoteShell::UDPCustomMessageID> *getUDPClient();
        ~Network();
        
    private:
        std::string _serverHost;
        FWNetwork::Asio::TCP::ClientBase<RemoteShell::TCPCustomMessageID> _networkTCP;
        FWNetwork::Asio::UDP::ClientBase<RemoteShell::UDPCustomMessageID> *_networkUDP;
};

#endif