#ifndef _ASIO_EXCEPTION_
#define _ASIO_EXCEPTION_
#include "FWNetworkException.hpp"

namespace FWNetwork {
namespace Asio {
namespace TCP {

    class ClientException : FWNetwork::Exception {
        public:
            ClientException(const std::string &message): FWNetwork::Exception("Asio/TCP/Client : " + message), _msg("Asio/TCP/Client : " + message) {};
            const char *what() const noexcept override { return _msg.c_str(); };
            ~ClientException() {};
        private:
            std::string _msg; 
    };

    class ServerException : FWNetwork::Exception {
        public:
            ServerException(const std::string &message): FWNetwork::Exception("Asio/TCP/Server : " + message), _msg("Asio/TCP/Server : " + message) {};
            const char *what() const noexcept override { return _msg.c_str(); };
            ~ServerException() {};
        private:
            std::string _msg; 
    };
};
};
};

#endif