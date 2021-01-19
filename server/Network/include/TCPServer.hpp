#ifndef TCPSERVER_HPP_
#define TCPSERVER_HPP_

#include "AsioTCPServer.hpp"
#include "CustomMessageIDs.hpp"

namespace RemoteShell {
class TCPServer : public FWNetwork::Asio::TCP::ServerBase<RemoteShell::TCPCustomMessageID> {
    public:
        TCPServer(unsigned short port) : FWNetwork::Asio::TCP::ServerBase<RemoteShell::TCPCustomMessageID>(port) {};
        ~TCPServer() {};

        virtual bool onClientConnect(boost::shared_ptr<FWNetwork::Asio::TCP::Connection<RemoteShell::TCPCustomMessageID>> client)
        {
            FWNetwork::Message<RemoteShell::TCPCustomMessageID> msg;

            msg.header.id = RemoteShell::TCPCustomMessageID::SERVER_ACCEPT;
            client->send(msg);
            std::cout << "New client connected TCP!" << std::endl;
            return (true);
        }

        virtual void onClientDisconnect(boost::shared_ptr<FWNetwork::Asio::TCP::Connection<RemoteShell::TCPCustomMessageID>> client)
        {
            std::cout << "Removing client [" << client->getID() << "]" << std::endl;
            client->disconnect();
        }

        virtual void onMessage(boost::shared_ptr<FWNetwork::Asio::TCP::Connection<RemoteShell::TCPCustomMessageID>> client, FWNetwork::Message<RemoteShell::TCPCustomMessageID> &msg)
        {
            if (client == nullptr) {

            } else {
                std::cout << msg << " from " << client->getID() << std::endl;
            }
            _msgList.push_back({client, msg});
        }

        FWNetwork::ThreadQueue<FWNetwork::OwnedMessageTCP<RemoteShell::TCPCustomMessageID>> &getMsgList()
        {
            return _msgList;
        }

    private:
        FWNetwork::ThreadQueue<FWNetwork::OwnedMessageTCP<RemoteShell::TCPCustomMessageID>> _msgList;
};
};

#endif /* !TCPSERVER_HPP_ */