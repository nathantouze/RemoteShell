
#ifndef ASIOUDPSERVER_HPP
#define ASIOUDPSERVER_HPP

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <map>
#include <array>
#include "Message.hpp"
#include "ThreadQueue.hpp"
#include "AsioUDPException.hpp"

namespace FWNetwork {
namespace Asio {
namespace UDP {

    template <typename T>
    class ServerBase {

        public:
            ServerBase(uint16_t port) : 
            _serverPort(port), 
            _serverEndpoint(boost::asio::ip::udp::v4(), port), 
            _socket(_asioContext, _serverEndpoint) {

            };

            virtual ~ServerBase() {
                stopServer();
            };

            uint16_t getServerPort(void) {
                return _serverPort;
            };

            void stopServer() {
                _asioContext.stop();
                if (_thread.joinable())
                    _thread.join();
            };

            void startServer() {
                try {
                    readMessageHeader();
                    _thread = std::thread([this]() {
                        _asioContext.run();
                    });
	            } catch (std::exception& err) {
					throw FWNetwork::Asio::UDP::ServerException(err.what());
                }
            };

            void readMessageHeader() {
                _socket.async_receive_from(boost::asio::buffer(&_dataTmp.header, sizeof(MessageHeader<T>)), _remoteEndpoint, 
                    [this](std::error_code err, std::size_t length) {
		            	if (!err)
                            if (_dataTmp.header.size > 0) {
                                _dataTmp.body.resize(_dataTmp.header.size - sizeof(MessageHeader<T>));
		            			readMessageBody();
		            		} else
                                addToQueue();
		            	else {
                            std::cout << "[UDP] Read Header Fail.\n";
		            	}
		            });
            };

            void readMessageBody() {
                _socket.async_receive_from(boost::asio::buffer(_dataTmp.body.data(), _dataTmp.body.size()), _remoteEndpoint,
                    [this](std::error_code err, std::size_t length) {						
		            	if (!err)
                            addToQueue();
		            	else {
                            std::cout << "[UDP] Read Body Fail.\n";
		            	}
		            });
            };

            void addToQueue(void) {
                _incomingMessages.push_back({ 
                    _remoteEndpoint, _dataTmp 
                });
                readMessageHeader();
            };

            uint32_t getClientId(boost::asio::ip::udp::endpoint endpoint)
            {
                auto cit = _connections.find(endpoint);
                if (cit != _connections.end())
                    return (*cit).second;

                if (onClientConnect(endpoint) == true) {
                    _idCounter++;
                    _connections.insert(std::pair<boost::asio::ip::udp::endpoint, uint32_t>(endpoint, _idCounter));
                    return _idCounter;
                }
                return (-1);
            };

            void updateServer(size_t nMaxMessages = -1) {
                size_t nMessageCount = 0;
                while (nMessageCount < nMaxMessages && !_incomingMessages.empty()) {
                    auto msg = _incomingMessages.pop_front();
                    onMessage(msg.remote, getClientId(msg.remote), msg.msg);
                    nMessageCount++;
                }
            };

            void writeMessageHeader() {
                _socket.send_to(boost::asio::buffer(&_dataOut.front().msg.header, sizeof(MessageHeader<T>)), _dataOut.front().remote);
            };

            void writeMessageBody() {
                _socket.send_to(boost::asio::buffer(_dataOut.front().msg.body.data(), _dataOut.front().msg.body.size()), _dataOut.front().remote);
            };

            void send(const Message<T>& message, boost::asio::ip::udp::endpoint target_endpoint) {
                _dataOut.push_back({target_endpoint, message});
                while (!_dataOut.empty()) {
                    writeMessageHeader();
                    if (_dataOut.front().msg.body.size() > 0)
                        writeMessageBody();
                    _dataOut.pop_front();
                }
            };

            void sendToClient(const Message<T>& message, uint32_t clientID) {
                for (auto it : _connections) {
                    if (it.second == clientID)
                        send(message, it.first);
                }
            };

            void sendToAllExcept(const Message<T> &message, uint32_t clientID) {
                for (auto client: _connections)
                    if (client.second != clientID)
                        send(message, client.first);
            };

            void sendToAll(const Message<T> &message) {
                for (auto client: _connections)
                    send(message, client.first);
            };

            const std::map<boost::asio::ip::udp::endpoint, uint32_t> &getClients() const
            {
                return _connections;
            }
        protected:
            uint16_t _serverPort;
            boost::asio::ip::udp::endpoint _serverEndpoint;
            boost::asio::ip::udp::endpoint _remoteEndpoint;
            boost::asio::io_context _asioContext;
            boost::asio::ip::udp::socket _socket;
            uint32_t _idCounter = 10000;
            std::map<boost::asio::ip::udp::endpoint, uint32_t> _connections;
            std::thread _thread;
            FWNetwork::Message<T> _dataTmp;
            FWNetwork::ThreadQueue<OwnedMessageUDP<T>> _dataOut;
            FWNetwork::ThreadQueue<OwnedMessageUDP<T>> _incomingMessages;

            virtual bool onClientConnect(boost::asio::ip::udp::endpoint target_endpoint) {return false;};
            virtual void onMessage(boost::asio::ip::udp::endpoint, int, FWNetwork::Message<T>&) {};
    };
};
};
};

#endif