
#ifndef ASIOUDPEXCEPTION_HPP
#define ASIOUDPEXCEPTION_HPP

#include "FWNetworkException.hpp"

namespace FWNetwork {
namespace Asio {
namespace UDP {

    class ClientException : FWNetwork::Exception {
        public:
            ClientException(const std::string &message): FWNetwork::Exception("Asio/UDP/Client : " + message), _msg("Asio/TCP/Client : " + message) {};
            const char *what() const noexcept override { return _msg.c_str(); };
            ~ClientException() {};
        private:
            std::string _msg; 
    };

    class ServerException : FWNetwork::Exception {
        public:
            ServerException(const std::string &message): FWNetwork::Exception("Asio/UDP/Server : " + message), _msg("Asio/TCP/Server : " + message) {};
            const char *what() const noexcept override { return _msg.c_str(); };
            ~ServerException() {};
        private:
            std::string _msg; 
    };
};
};
};

#endif