#ifndef UDPSERVER_HPP_
#define UDPSERVER_HPP_

#include "AsioUDPServer.hpp"
#include "CustomMessageIDs.hpp"

namespace RemoteShell {
class UDPServer : public FWNetwork::Asio::UDP::ServerBase<UDPCustomMessageID>{
    public:
        UDPServer(unsigned int port) : FWNetwork::Asio::UDP::ServerBase<UDPCustomMessageID>(port), _port(port) {};
        ~UDPServer() {};

        virtual bool onClientConnect(boost::asio::ip::udp::endpoint target_endpoint) {
            if (_nbClient < 4) {
                std::cout << "New client connected UDP on " << _port << " !" << std::endl;
                _nbClient++;
                return true;
            }
            return false;
        };

        virtual void onMessage(boost::asio::ip::udp::endpoint endpoint, int id, FWNetwork::Message<RemoteShell::UDPCustomMessageID>& msg) {
            _msgList.push_back({endpoint, msg});
        };

        FWNetwork::ThreadQueue<FWNetwork::OwnedMessageUDP<UDPCustomMessageID>> &getMsgList() {
            return _msgList;
        };

        const uint32_t &getPort() const {
            return _port;
        };

    private:
        int _nbClient = 0;
        uint32_t _port;
        FWNetwork::ThreadQueue<FWNetwork::OwnedMessageUDP<UDPCustomMessageID>> _msgList;
};
};
#endif /* !UDPSERVER_HPP_ */
