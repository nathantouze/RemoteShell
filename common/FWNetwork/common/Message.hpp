#ifndef MESSAGE_HPP_
#define MESSAGE_HPP_

#include <vector>
#include <memory>
#include <iostream>
#include <cstring>

namespace FWNetwork
{
    template <typename T>
    struct MessageHeader {
        T id{};
        size_t size = 0;
    };

    template <typename T>
    struct Message {
        MessageHeader<T> header {};
        std::vector<char> body;

        size_t getSize() const {
            return sizeof(MessageHeader<T>) + body.size();
        }

        template <typename Data>
        friend Message<T>& operator << (Message<T>& message, const Data& data) {
            size_t size = message.body.size();
            message.body.resize(message.body.size() + sizeof(Data));
            std::memcpy(message.body.data() + size, &data, sizeof(Data));
            message.header.size = message.getSize();
            return message;
        }

        friend std::ostream& operator << (std::ostream &os, const Message<T> &message) {
            os << "ID: " << int(message.header.id) << " Size: " << message.header.size;
            return os;
        }

        template <typename Data>
        friend Message<T>& operator >> (Message<T>& message, Data& data) {
            size_t size = message.body.size() - sizeof(Data);
            std::memcpy(&data, message.body.data() + size, sizeof(Data));
            message.body.resize(size);
            return message;
        }
    };

    namespace Asio {
        namespace TCP {
            template <typename T>
	        class Connection;
        }
    }

    template <typename T>
	struct OwnedMessageTCP
	{
		boost::shared_ptr<Asio::TCP::Connection<T>> remote = nullptr;
		Message<T> msg;
		friend std::ostream& operator<<(std::ostream& os, const OwnedMessageTCP<T>& msg)
		{
			os << msg.msg;
			return os;
		}
	};

    template <typename T>
	struct OwnedMessageUDP
	{
		boost::asio::ip::udp::endpoint remote = nullptr;
		Message<T> msg;
		friend std::ostream& operator<<(std::ostream& os, const OwnedMessageUDP<T>& msg)
		{
			os << msg.msg;
			return os;
		}
	};
}


#endif /* !MESSAGE_HPP_ */
