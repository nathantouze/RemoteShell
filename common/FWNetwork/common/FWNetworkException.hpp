#ifndef _FW_NETWORK_EXCEPTION_
#define _FW_NETWORK_EXCEPTION_

#include <exception>
#include <string>

namespace FWNetwork {

    class Exception : std::exception {
        public:
            Exception(const std::string &message): _msg(message) {};
            const char *what() const noexcept override {
                return _msg.c_str();
            };
            ~Exception() {};

        protected:
        private:
            std::string _msg; 
    };
};

#endif