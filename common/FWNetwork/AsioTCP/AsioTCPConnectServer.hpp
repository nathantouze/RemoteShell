
#ifndef ASIOTCPCONNECTSERVER_HPP
#define ASIOTCPCONNECTSERVER_HPP

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/make_shared.hpp>
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

            Connection(Owner parent, boost::asio::io_context& asioContext, boost::asio::ip::tcp::socket socket, /*std::list<std::string>& qIn*/TsQueue<OwnedMessage<T>>& qIn)
            : _asioContext(asioContext), _socket(std::move(socket)), _messagesIn(qIn)
            {
                _ownerType = parent;
            }

            static boost::shared_ptr<Connection<T>> create(Owner parent, boost::asio::io_context& io_context, TsQueue<OwnedMessage<T>>& qIn) {
                boost::shared_ptr<Connection<T>> myPointer(new Connection<T>(parent, io_context, qIn));
                return (myPointer);
            }

            boost::asio::ip::tcp::socket& getSocket() 
            {
                return _socket;
            }

            virtual ~Connection() {};

            unsigned int getID() const {
                return _id;
            };

            void connectToClient(unsigned int uid = 0) {
                if (_ownerType == Owner::SERVER) {
	            	if (_socket.is_open()) {
	            		_id = uid;
	            		readHeader();
	            	}
	            }
            };


 

            void disconnect() {
                if (isConnected()) {
                    if (_ownerType == Owner::CLIENT) {
                        std::cout << "[CLIENT 2.0] : Client is disconnected" << std::endl;
                        boost::asio::post(_asioContext, [this]() { 
                            _socket.close();
                        });
                    } else {
                        std::cout << "[SERVER 2.0] : Client [" << _id << "] disconnected" << std::endl;
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
                        if (_ownerType == Owner::SERVER) {
                        }

		            	if (!newMessage) {
                            std::cout << "I'm the server, I send a message" << std::endl;
                            writeHeader();
		            	}

            };



        protected:
			boost::asio::ip::tcp::socket _socket;
			boost::asio::io_context& _asioContext;
			ThreadQueue<Message<T>> _messagesOut;
            //std::list<std::string>& _messagesIn;
			ThreadQueue<OwnedMessage<T>>& _messagesIn;
			Message<T> _tmpMessage;

            std::vector<char> _tempData;
            unsigned int _tempSize;
            //std::string _tmpMessage;
            //char _data[10000];
			OwnerType _ownerType = Owner::SERVER;
			unsigned int _id = 0;

        private:

            void writeHeader() {
                std::cout << "I want to write an header" << std::endl;
                boost::asio::async_write(_socket, boost::asio::buffer(&_messagesOut.front().header, sizeof(MessageHeader<T>)),
                    [this](std::error_code ec, std::size_t length) {
                        std::cout << "I've write an header of " << length << " bytes" << std::endl;
                        if (!ec) {
                            if (_messagesOut.front().body.size() > 0)
                                writeBody();
		            		else {
                                std::cout << "No body to write" << std::endl;
                                _messagesOut.pop_front();
                                if (!_messagesOut.empty()) {
                                    writeHeader();
		            			}
		            		}
		            	} else {
                            std::cout << "[" << _id << "] Write Header Fail.\n";
		            		_socket.close();
		            	}
		            });
            };

            void writeBody() {
                std::cout << "I want to write a body of size " << _messagesOut.front().body.size() << std::endl;
                std::cout << _messagesOut.front().body.data() << std::endl;
                boost::asio::async_write(_socket, boost::asio::buffer(_messagesOut.front().body.data(), _messagesOut.front().body.size()),
                    [this](std::error_code ec, std::size_t length) {
                        std::cout << "I've write a body of " << length << " bytes" << std::endl;
                        if (!ec) {
                            _messagesOut.pop_front();
                            if (!_messagesOut.empty()) {
                                writeHeader();
                            }
		            	} else {
                            std::cout << "[" << _id << "] Write Body Fail.\n";
		            		_socket.close();
		            	}
		            });
            };
            
            
            void readHeader() {
                boost::asio::async_read(_socket, boost::asio::buffer(&_tmpMessage.header, sizeof(MessageHeader<T>)),
                    [this](std::error_code ec, std::size_t length) {						
		            	if (!ec) {
                            std::cout << "I've want to read an header ! " << std::endl;
                            if (_tmpMessage.header.size > 0) {
                                _tmpMessage.body.resize(_tmpMessage.header.size - sizeof(MessageHeader<T>));
		            			readBody();
		            		} else {
                                addToIncomingMessageQueue();
		            		}
		            	} else {
                            std::cout << "[" << _id << "] Read Header Fail.\n";
		            		_socket.close();
		            	}
		            });
            };

            void readBody() {
                boost::asio::async_read(_socket, boost::asio::buffer(_tmpMessage.body.data(), _tmpMessage.body.size()),
                    [this](std::error_code ec, std::size_t length) {
                        std::cout << "I've want to read a body ! " << std::endl;						
		            	if (!ec) {
                            addToIncomingMessageQueue();
		            	} else {
                            std::cout << "[" << _id << "] Read Body Fail.\n";
		            		_socket.close();
		            	}
		            });
            };
           
            void addToIncomingMessageQueue() {
                if(_ownerType == Owner::SERVER) {
                    OwnedMessage<T> message{this->shared_from_this(), _tmpMessage};
	                _messagesIn.push_back(message);
                }

                readHeader();
            };
            
    };

};
};
};

#endif