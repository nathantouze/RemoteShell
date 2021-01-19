#include "CoreServer.hpp"
#include "CustomDefines.hpp"
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

CoreServer::CoreServer(int port) : _network(port), _fpsManager(30.0f), _running(true)
{
}

void CoreServer::start()
{
    try {
        _fpsManager.start();
        _network.startTCPServer();
        while (_running) {
            if (_fpsManager.isTimePassed()) {
                _network.update();
                if (!_network.getMessagesFromTCP().empty())
                    analyse_messages();
            }
        }
    } catch (std::exception &err) {
        std::cout << err.what() << std::endl;
    }
}

const std::string CoreServer::execute_shell_command(const char *cmd)
{
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&PCLOSE)> pipe(POPEN(cmd, "r"), PCLOSE);

    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

void CoreServer::analyse_messages()
{
    while (!_network.getMessagesFromTCP().empty()) {
        auto msgGet = _network.getMessagesFromTCP().pop_front();
        auto msg = msgGet.msg;
        switch (msg.header.id) {
            case RemoteShell::TCPCustomMessageID::CLIENT_DISCONNECTED: {
                std::cout << "Disconnecting client [" << msgGet.remote << "]" << std::endl;
                msgGet.remote->disconnect();
                break;
            }
            case RemoteShell::TCPCustomMessageID::SHELL_CMD: {
                char cmd[MAX_CMD_LENGTH];
                char output[MAX_OUTPUT_LENGTH];
                FWNetwork::Message<RemoteShell::TCPCustomMessageID> answer;
                std::string output_str;

                msg >> cmd;
                output_str = execute_shell_command(cmd);
                for (unsigned int i = 0; i < MAX_OUTPUT_LENGTH; i++) {
                    if (i >= output_str.size())
                        output[i] = '\0';
                    else
                        output[i] = output_str[i];
                }
                output[MAX_OUTPUT_LENGTH - 1] = '\0';
                answer.header.id = RemoteShell::TCPCustomMessageID::SHELL_OUTPUT;
                answer << output;
                msgGet.remote->send(answer);
                break;
            }
            default: {
                break;
            }
        }
    }
}