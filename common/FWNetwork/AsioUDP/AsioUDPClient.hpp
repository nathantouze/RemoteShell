
#ifndef ASIOUDPCLIENT_HPP
#define ASIOUDPCLIENT_HPP

#include <boost/asio.hpp>
#include <thread>
#include <boost/bind.hpp>
#include "ThreadQueue.hpp"
#include "Message.hpp"
#include "AsioUDPException.hpp"

namespace FWNetwork {
namespace Asio {
namespace UDP {

    template<typename T>
    class ClientBase {
        public:
            ClientBase(std::string host, std::string server_port, unsigned short local_port = 0):
            _host(host),
            _serverPort(server_port),
            _socket(_asioContext, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), local_port)) {

            };

            ~ClientBase() {
                _asioContext.stop();
				if (_thread.joinable())
					_thread.join();
            };

            void startClient() {
                boost::asio::ip::udp::resolver resolver(_asioContext);
                boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), _host, _serverPort);
                _serverEndpoint = *resolver.resolve(query);
                readMessageHeader();
                _thread = std::thread([this]() { 
                    _asioContext.run();
                });
            };

            void readMessageHeader() {
                _socket.async_receive_from(boost::asio::buffer(&_dataTmp.header, sizeof(MessageHeader<T>)), _serverEndpoint, 
                    [this](std::error_code err, std::size_t length) {						
		            	if (!err)
                            if (_dataTmp.header.size > 0) {
                                _dataTmp.body.resize(_dataTmp.header.size - sizeof(MessageHeader<T>));
		            			readMessageBody();
		            		} else
                                addToQueue();
		            	else {
                            std::cout << " Read Header Fail.\n";
		            	}
		            });
            };

            void readMessageBody() {
                _socket.async_receive_from(boost::asio::buffer(_dataTmp.body.data(), _dataTmp.body.size()), _serverEndpoint,
                    [this](std::error_code err, std::size_t length) {						
		            	if (!err)
                            addToQueue();
		            	else {
                            std::cout << " Read Body Fail.\n";
		            	}
		            });
            };

            void addToQueue() {
                _incomingMessages.push_back({ 
                    _dataTmp 
                });
                readMessageHeader();
            };

            void send(const Message<T> &message) {
		        _dataOut.push_back(message);
                    while (!_dataOut.empty()) {
                        writeMessageHeader();
                        if (_dataOut.front().body.size() > 0)
                            writeMessageBody();
                        _dataOut.pop_front();
                    }
            };

            void writeMessageHeader() {
                _socket.send_to(boost::asio::buffer(&_dataOut.front().header, sizeof(MessageHeader<T>)), _serverEndpoint);
            };

            void writeMessageBody() {
                _socket.send_to(boost::asio::buffer(_dataOut.front().body.data(), _dataOut.front().body.size()), _serverEndpoint);
            };

            ThreadQueue<Message<T>>& incomingMessage() {
                return _incomingMessages;
            };

        protected:  
            std::string _host;
            std::string _serverPort;
            boost::asio::io_context _asioContext;
            boost::asio::ip::udp::socket _socket;
            boost::asio::ip::udp::endpoint _serverEndpoint;
            boost::asio::ip::udp::endpoint _remoteEndpoint;
            Message<T> _dataTmp;
            std::thread _thread;
            FWNetwork::ThreadQueue<Message<T>> _incomingMessages;
            FWNetwork::ThreadQueue<Message<T>> _dataOut;
    };

};
};
};

#endif