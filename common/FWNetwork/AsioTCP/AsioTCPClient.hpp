#ifndef _ASIO_TCP_CLIENT_
#define _ASIO_TCP_CLIENT_

#include <boost/make_shared.hpp>
#include "AsioTCPConnection.hpp"

namespace FWNetwork {
namespace Asio {
namespace TCP {

    template <typename T>
    class ClientBase {
        public:
            ClientBase() {};

            virtual ~ClientBase() {
                disconnect();
            };

            void connectToServer(const std::string& host, int port) {
				try {
					boost::asio::ip::tcp::resolver resolver(_context);
					boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));
					_connection = std::make_unique<Connection<T>>(Connection<T>::OwnerType::CLIENT, _context, boost::asio::ip::tcp::socket(_context), _messagesIn);
		
					_connection->connectToServer(endpoints);

					_thread = std::thread([this]() { 
                        _context.run();
                    });
				} catch (std::exception& err) {
					throw ClientException(err.what());
				}
			};

            void disconnect() {
				if (isConnected())
					_connection->disconnect();
				_context.stop();
				if (_thread.joinable())
					_thread.join();
				_connection.release();
			}

            bool isConnected() const {
				if (_connection)
					return _connection->isConnected();
				else
					return false;
			};

			void send(const Message<T>& msg) {
				if (isConnected())
					_connection->send(msg);
			};

			ThreadQueue<OwnedMessageTCP<T>>& incomingMessage()
			{ 
				return _messagesIn;
			}

        protected:
			boost::asio::io_context _context;
			std::thread _thread;
			std::unique_ptr<Connection<T>> _connection;

        private:
			ThreadQueue<OwnedMessageTCP<T>> _messagesIn;
    };

};
};
};

#endif