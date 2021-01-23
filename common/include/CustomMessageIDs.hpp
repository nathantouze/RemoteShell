#ifndef _CUSTOM_MESSAGES_
#define _CUSTOM_MESSAGES_

#include <cstdint>

namespace RemoteShell {

    enum TCPCustomMessageID : uint32_t {
        SERVER_ACCEPT,              //Body : 
        SERVER_DISCONNECTED,        //Body : 
        CLIENT_DISCONNECTED,        //Body : 
        INFORMATION_ASK,            //Body :
        INFORMATION_SEND,           //Body : char[MAX_OS_LENGTH] os << char[MAX_PWD_LENGTH];
        SHELL_CMD,                  //Body : char[MAX_CMD_LENGTH] cmd;
        SHELL_OUTPUT,               //Body : char[MAX_OUTPUT_LENGTH] output;
        CHANGE_DIRECTORY_IN,        //Body : char[MAX_CMD_LENGTH] cmd;
        CHANGE_DIRECTORY_OUT,       //Body : char[MAX_OUTPUT_LENGTH] output << char[MAX_PWD_LENGTH] newPath;
    };

    enum UDPCustomMessageID : uint32_t {
        CLIENT_CONNECTED,           //Body : 
        CLIENT_DISCONNECTED_UDP,    //Body : 
        SERVER_CLIENT_DISCONNECTED, //Body : 
    };
};

#endif