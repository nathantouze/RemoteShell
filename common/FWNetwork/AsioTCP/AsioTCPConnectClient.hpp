

#ifndef ASIOTCPCOMPONENTCLIENT_HPP
#define ASIOTCPCOMPONENTCLIENT_HPP

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <memory>
#include "Message.hpp"
#include "ThreadQueue.hpp"
#include <iostream>

namespace FWNetwork {
namespace Asio {
namespace TCP {

    template <typename T>
    class Connection : public boost::enable_shared_from_this<Connection<T>> {
        public:
            enum class Owner {
                SERVER,
                CLIENT
            };

            Connection(Owner parent, boost::asio::io_context& asioContext, boost::asio::ip::tcp::socket socket, TsQueue<OwnedMessage<T>>& qIn/*std::list<std::string>& qIn*/)
            : _asioContext(asioContext), _socket(std::move(socket)), _messagesIn(qIn)
            {
                _ownerType = parent;
            }

            boost::asio::ip::tcp::socket& getSocket() 
            {
                return _socket;
            }

            virtual ~Connection() {};

            unsigned int getID() const {
                return _id;
            };


            void connectToServer(const boost::asio::ip::tcp::resolver::results_type& endpoints) {
                if (_ownerType == Owner::CLIENT) {
	            	boost::asio::async_connect(_socket, endpoints,
	            	[this](std::error_code ec, boost::asio::ip::tcp::endpoint endpoint) {
	            		if (!ec) {
                            readHeader();
	            		}
	            	});
	            }
            };


            void disconnect() {
                if (isConnected()) {
                    if (_ownerType == Owner::CLIENT) {
                        boost::asio::post(_asioContext, [this]() { 
                            _socket.close();
                        });
                    }
                }
            };

            bool isConnected() const {
                return _socket.is_open();
            };

            void startListening() {};

            void send(const Message<T>& msg/*const std::string &message*/) {

                bool newMessage = !_messagesOut.empty();
                _messagesOut.push_back(msg);
                if (!newMessage) {
                    std::cout << "I'm the client, I want to send a message" << std::endl;
                    writeHeader();
                    std::cout << "I'm the client, I sended a message" << std::endl;
                }
            };


        protected:
			boost::asio::ip::tcp::socket _socket;
			boost::asio::io_context& _asioContext;
			ThreadQueue<Message<T>> _messagesOut;
			ThreadQueue<OwnedMessage<T>>& _messagesIn;
			Message<T> _tmpMessage;
            std::vector<char> _tempData;
			Owner _ownerType = Owner::CLIENT;
			unsigned int _id = 0;

        private:
            
            void writeHeader() {
                boost::asio::async_write(_socket, boost::asio::buffer(&_messagesOut.front().header, sizeof(MessageHeader<T>)),
                    [this](std::error_code ec, std::size_t length) {
                        if (!ec) {
                            if (_messagesOut.front().body.size() > 0) {
                                writeBody();
		            		} else {
                                _messagesOut.pop_front();
                                if (!_messagesOut.empty()) {
                                    writeHeader();
		            			}
		            		}
		            	} else {
		            		_socket.close();
		            	}
		            });
            };

            void writeBody() {
                boost::asio::async_write(_socket, boost::asio::buffer(_messagesOut.front().body.data(), _messagesOut.front().body.size()),
                    [this](std::error_code ec, std::size_t length) {
                        if (!ec) {
                            _messagesOut.pop_front();
                            if (!_messagesOut.empty()) {
                                writeHeader();
                            }
		            	} else {
		            		_socket.close();
		            	}
		            });
            };

            void readEverything() {
                _tempData.resize(12);
                boost::asio::async_read(_socket, boost::asio::buffer(_tempData.data(), _tempData.size()),
                    [this](std::error_code ec, std::size_t length) {
		            	if (!ec) {

                            for (auto it : _tempData) {
                                std::cout << (int)(it) << " ";
                            }
                            readEverything();



		            	} else {
		            		_socket.close();
		            	}
		            });
            };


            void readHeader() {
                boost::asio::async_read(_socket, boost::asio::buffer(&_tmpMessage.header, sizeof(MessageHeader<T>)),
                    [this](std::error_code ec, std::size_t length) {
		            	if (!ec) {
                            if (_tmpMessage.header.size > 0) {
                                _tmpMessage.body.resize(_tmpMessage.header.size - sizeof(MessageHeader<T>));
		            			readBody();
		            		} else {
                                addToIncomingMessageQueue();
		            		}
		            	} else {
		            		_socket.close();
		            	}
		            });
            };

            void readBody() {
                boost::asio::async_read(_socket, boost::asio::buffer(_tmpMessage.body.data(), _tmpMessage.body.size()),
                    [this](std::error_code ec, std::size_t length) {		
		            	if (!ec) {
                            addToIncomingMessageQueue();
		            	} else {
		            		_socket.close();
		            	}
		            });
            };  
           
            void addToIncomingMessageQueue() {
	            _messagesIn.push_back({ 
                    nullptr, _tmpMessage 
                });
                readHeader();
            };
            
    };

};
};
};

#endif