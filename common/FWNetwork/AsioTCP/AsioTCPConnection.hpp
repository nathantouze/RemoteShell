#ifndef _ASIO_TCP_CONNECTION_
#define _ASIO_TCP_CONNECTION_

#include <boost/asio.hpp>
#include <memory>
#include <boost/enable_shared_from_this.hpp>
#include <boost/make_shared.hpp>
#include "Message.hpp"
#include "ThreadQueue.hpp"
#include "AsioTCPException.hpp"

namespace FWNetwork {
namespace Asio {
namespace TCP {

    template <typename T>
    class Connection : public boost::enable_shared_from_this<Connection<T>> {
        public:
            enum class OwnerType {
                SERVER,
                CLIENT
            };

            Connection(OwnerType parent, boost::asio::io_context& ioContext, boost::asio::ip::tcp::socket socket, ThreadQueue<OwnedMessageTCP<T>>& qIn)
            : _ioContext(ioContext), _socket(std::move(socket)), _dataIn(qIn)
            {
                _ownerType = parent;
            }

            virtual ~Connection() {};

            unsigned int getID() const {
                return _id;
            };

            void disconnect() {
                if (isConnected())
                    boost::asio::post(_ioContext, [this]() { 
                        _socket.close();
                    });
            };

            bool isConnected() const {
                return _socket.is_open();
            };

            void send(const Message<T>& msg) {
                boost::asio::post(_ioContext,
                    [this, msg]() {
                        bool newMessage = !_dataOut.empty();
		            	_dataOut.push_back(msg);
		            	if (!newMessage)
                            writeMessageHeader();
	                });
            };

            void connectToClient(unsigned int uid = 0) {
                if (_ownerType == OwnerType::SERVER) {
	            	if (_socket.is_open()) {
	            		_id = uid;
	            		readMessageHeader();
	            	}
	            }
            };

            void connectToServer(const boost::asio::ip::tcp::resolver::results_type& endpoints) {
                if (_ownerType == OwnerType::CLIENT)
	            	boost::asio::async_connect(_socket, endpoints,
	            	[this](std::error_code err, boost::asio::ip::tcp::endpoint endpoint) {
	            		if (!err)
                            readMessageHeader();
	            	});
            };

        private:
            void writeMessageHeader() {
                boost::asio::async_write(_socket, boost::asio::buffer(&_dataOut.front().header, sizeof(MessageHeader<T>)),
                    [this](std::error_code err, std::size_t length) {
                        if (!err) {
                            if (_dataOut.front().body.size() > 0)
                                writeMessageBody();
		            		else {
                                _dataOut.pop_front();
                                if (!_dataOut.empty()) {
                                    writeMessageHeader();
		            			}
		            		}
		            	} else {
		            		disconnect();
		            	}
		            });
            };

            void writeMessageBody() {
                boost::asio::async_write(_socket, boost::asio::buffer(_dataOut.front().body.data(), _dataOut.front().body.size()),
                    [this](std::error_code err, std::size_t length) {
                        if (!err) {
                            _dataOut.pop_front();
                            if (!_dataOut.empty()) {
                                writeMessageHeader();
                            }
		            	} else {
		            		disconnect();
		            	}
		            });
            };

            void readMessageHeader() {
                boost::asio::async_read(_socket, boost::asio::buffer(&_dataTmp.header, sizeof(MessageHeader<T>)),
                    [this](std::error_code err, std::size_t length) {						
		            	if (!err)
                            if (_dataTmp.header.size > 0) {
                                _dataTmp.body.resize(_dataTmp.header.size - sizeof(MessageHeader<T>));
		            			readMessageBody();
		            		} else
                                addToQueue();
		            	else {
		            		disconnect();
		            	}
		            });
            };

            void readMessageBody() {
                boost::asio::async_read(_socket, boost::asio::buffer(_dataTmp.body.data(), _dataTmp.body.size()),
                    [this](std::error_code err, std::size_t length) {						
		            	if (!err)
                            addToQueue();
		            	else {
                            std::cout << "[" << _id << "] Read Body Fail.\n";
		            		disconnect();
		            	}
		            });
            };

            void addToQueue() {
                if (_ownerType == OwnerType::SERVER) {
                    _dataIn.push_back({
                        this->shared_from_this(), _dataTmp 
                    });
                    _dataTmp.body.resize(0);
                } else {
	                _dataIn.push_back({ 
                        nullptr, _dataTmp 
                    });
                }
                readMessageHeader();
            };

			OwnerType _ownerType = OwnerType::SERVER;
			ThreadQueue<OwnedMessageTCP<T>>& _dataIn;
			boost::asio::ip::tcp::socket _socket;
			boost::asio::io_context& _ioContext;
			ThreadQueue<Message<T>> _dataOut;
			unsigned int _id = 0;
			Message<T> _dataTmp;
    };

};
};
};

#endif