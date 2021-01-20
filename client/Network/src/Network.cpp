
#include "Network.hpp"

Network::Network(void) 
{

};

void Network::startTCPClient(std::string host, std::string port)
{
    _serverHost = host;
    if (!_networkTCP.isConnected())
        _networkTCP.connectToServer(host, std::stoi(port));
};

void Network::startUDPClient(uint32_t port)
{
    _networkUDP = new FWNetwork::Asio::UDP::ClientBase<RemoteShell::UDPCustomMessageID>(_serverHost, std::to_string(port));
    _networkUDP->startClient();

    FWNetwork::Message<RemoteShell::UDPCustomMessageID> message;
    message.header.id = RemoteShell::UDPCustomMessageID::CLIENT_CONNECTED;
    _networkUDP->send(message);
};

bool Network::isConnectedToTCP() const 
{
    return _networkTCP.isConnected();
};

FWNetwork::ThreadQueue<FWNetwork::OwnedMessageTCP<RemoteShell::TCPCustomMessageID>>& Network::getMessagesFromTCP()
{
    return _networkTCP.incomingMessage();
};

void Network::sendMessagesToTCP(std::vector<FWNetwork::Message<RemoteShell::TCPCustomMessageID>>& messages)
{
    if (_networkTCP.isConnected()) {
        for (auto it : messages) {
            _networkTCP.send(it);
        }
        messages.clear();
    }
};

void Network::stopTCPClient()
{
    if (_networkTCP.isConnected())
        _networkTCP.disconnect();
};

FWNetwork::Asio::UDP::ClientBase<RemoteShell::UDPCustomMessageID> *Network::getUDPClient()
{
    return _networkUDP;
}

void Network::stopUDPClient()
{
    if (_networkUDP != nullptr) {
        FWNetwork::Message<RemoteShell::UDPCustomMessageID> msg;
        msg.header.id = RemoteShell::UDPCustomMessageID::CLIENT_DISCONNECTED_UDP;
        _networkUDP->send(msg);
        delete _networkUDP;
    }
}

Network::~Network(void) 
{
    stopTCPClient();
    stopUDPClient();
};