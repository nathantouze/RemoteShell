
#ifndef ASIOTCPSERVER_HPP
#define ASIOTCPSERVER_HPP

#include <iostream>
#include "AsioTCPConnection.hpp"

namespace FWNetwork {
namespace Asio {
namespace TCP {
    template <typename T>
    class ServerBase {
        public:
            ServerBase(unsigned short port): _acceptor(_asioContext, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
            {};

            virtual ~ServerBase() {
                stopServer();
            };

            void stopServer() {
                _asioContext.stop();
                if (_thread.joinable())
                    _thread.join();
                std::cout << "[SERVER] Stopped!\n";
            };

            void startServer() {
                try {
                    waitForConnection();
                    _thread = std::thread([this]() {
                        _asioContext.run();
                    });
	            } catch (std::exception& err) {
					throw ServerException(err.what());
                }
                std::cout << "[SERVER] Started on " << _acceptor.local_endpoint() << std::endl;
            };

            void waitForConnection() {
                _acceptor.async_accept(
                    [this](std::error_code ec, boost::asio::ip::tcp::socket socket) {
                        if (!ec) {
                            std::cout << "[SERVER] New Connection: " << socket.remote_endpoint() << std::endl;
                            boost::shared_ptr<Connection<T>> newconn = boost::make_shared<Connection<T>>(Connection<T>::OwnerType::SERVER, _asioContext, std::move(socket), _dataIn);
                            if (onClientConnect(newconn)) {
                                _connections.push_back(std::move(newconn));
                                _connections.back()->connectToClient(_idCounter++);
                                std::cout << "[" << _connections.back()->getID() << "] Connection Approved\n";
		            		} else
		            			std::cout << "[-----] Connection Denied\n";
		            	} else
					        throw ServerException("Connection failure");
                        waitForConnection();
		            });
            };

            void updateServer(size_t nMaxMessages = -1) {
                size_t nMessageCount = 0;
                while (nMessageCount < nMaxMessages && !_dataIn.empty()) {
                    auto msg = _dataIn.pop_front();
                    onMessage(msg.remote, msg.msg);
                    nMessageCount++;
                }
            };

        protected:
            boost::asio::io_context _asioContext;
            boost::asio::ip::tcp::acceptor _acceptor;
            std::deque<boost::shared_ptr<Connection<T>>> _connections;
            unsigned int _idCounter = 10000;
            std::thread _thread;
            ThreadQueue<OwnedMessageTCP<T>> _dataIn;

            virtual bool onClientConnect(boost::shared_ptr<Connection<T>> client) {return false;};
            virtual void onClientDisconnect(boost::shared_ptr<Connection<T>> client) {};
            virtual void onMessage(boost::shared_ptr<Connection<T>> client, Message<T>& msg) {};

    };
};
};
};

#endif