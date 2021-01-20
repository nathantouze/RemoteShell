#include "ClientCore.hpp"
#include "CustomDefines.hpp"
#include "CLI.hpp"
#include <vector>

ClientCore::ClientCore(const std::string &host, const std::string &port) : _host(host), _port(port), _running(true), _fpsManager(30.0f)
{
}

ClientCore::~ClientCore()
{
}

void ClientCore::start()
{
    CLI cli;
    std::string input;

    _network.startTCPClient(_host, _port);
    if (_network.isConnectedToTCP())
        ask_informations();
    _fpsManager.start();
    while (_running) {
        if (_fpsManager.isTimePassed() && _network.isConnectedToTCP()) {
            if (!_network.getMessagesFromTCP().empty())
                analyse_messages_from_TCP();
            if (!_running)
                return;
        }
        input = cli.getline();
        if (!input.empty()) {
            send_command_to_TCP_server(input);
        }
    }
}

void ClientCore::ask_informations()
{
    std::vector<FWNetwork::Message<RemoteShell::TCPCustomMessageID>> messages;
    FWNetwork::Message<RemoteShell::TCPCustomMessageID> msg;

    msg.header.id = RemoteShell::TCPCustomMessageID::INFORMATION_ASK;
    messages.push_back(msg);
    _network.sendMessagesToTCP(messages);
}

void ClientCore::send_command_to_TCP_server(const std::string &input)
{
    char cmd[MAX_CMD_LENGTH];
    std::vector<FWNetwork::Message<RemoteShell::TCPCustomMessageID>> messages;
    FWNetwork::Message<RemoteShell::TCPCustomMessageID> msg;

    if (input == "exit") {
        msg.header.id = RemoteShell::TCPCustomMessageID::CLIENT_DISCONNECTED;
        _running = false;
        std::cout << "Disconnecting client" << std::endl;
        messages.push_back(msg);
        _network.sendMessagesToTCP(messages);
        return;
    }
    msg.header.id = RemoteShell::TCPCustomMessageID::SHELL_CMD;
    for (unsigned int i = 0; i < MAX_CMD_LENGTH; i++) {
        if (i >= input.size())
            cmd[i] = '\0';
        else
            cmd[i] = input[i];
    }
    cmd[MAX_CMD_LENGTH - 1] = '\0';
    msg << cmd;
    messages.push_back(msg);
    _network.sendMessagesToTCP(messages);
}

void ClientCore::analyse_messages_from_TCP()
{
    while (!_network.getMessagesFromTCP().empty()) {
        auto msg = _network.getMessagesFromTCP().pop_front().msg;
        switch (msg.header.id) {
            case RemoteShell::TCPCustomMessageID::SERVER_ACCEPT: {
                std::cout << "Connected to the server." << std::endl;
                std::cout << "> ";
                break;
            }
            case RemoteShell::TCPCustomMessageID::SERVER_DISCONNECTED: {
                std::cout << "Server disconnected. Closing client..." << std::endl;
                _running = false;
                break;
            }
            case RemoteShell::TCPCustomMessageID::SHELL_OUTPUT: {
                char output[MAX_OUTPUT_LENGTH];

                msg >> output;
                std::cout << output << std::endl;
                std::cout << "> ";
                break;
            }
            default: {
                break;
            }
        }
    }
}