#include "NetworkServer.hpp"

NetworkServer::NetworkServer(unsigned int tcp_port) : _tcpServer(tcp_port)
{
}

NetworkServer::~NetworkServer()
{
}

void NetworkServer::startTCPServer()
{
    _tcpServer.startServer();
}

void NetworkServer::update()
{
    _tcpServer.updateServer(-1);
    for (auto it : _udpServers) {
        it->updateServer(-1);
    }
};

FWNetwork::ThreadQueue<FWNetwork::OwnedMessageTCP<RemoteShell::TCPCustomMessageID>> &NetworkServer::getMessagesFromTCP()
{
    return _tcpServer.getMsgList();
}

void NetworkServer::stopTCPServer()
{
    _tcpServer.stopServer();
}

void NetworkServer::stopUDPServer(uint32_t port)
{
    int i = 0;
    for (auto it : _udpServers) {
        if (it->getServerPort() == port) {
            it->stopServer();
            _udpServers.erase(_udpServers.begin()+i);
            return;
        }
        i++;
    }
}

std::shared_ptr<RemoteShell::UDPServer> NetworkServer::getUDPServer(uint32_t port)
{
    auto it = _udpServers.begin();

    while (it != _udpServers.end()) {
        if (it->get()->getPort() == port)
            break;
        it++;
    }
    return (*it);
}

uint32_t NetworkServer::startNewUDPServer() 
{
    uint32_t port = uint32_t(_udpServers.size() + 60000);
    _udpServers.push_back(std::make_shared<RemoteShell::UDPServer>(port));
    _udpServers.back()->startServer();
    return port;
}

std::vector<std::shared_ptr<RemoteShell::UDPServer>> &NetworkServer::getUDPServers()
{
    return _udpServers;
}