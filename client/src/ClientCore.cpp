#include "ClientCore.hpp"
#include "CustomDefines.hpp"
#include "CLI.hpp"
#include <vector>
#include <algorithm>

ClientCore::ClientCore(const std::string &host, const std::string &port) : _host(host), _port(port), _pwd(), _os("Unknown"), _running(true), _fpsManager(30.0f)
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
        if (!input.empty())
            handle_input(input);
    }
}

void ClientCore::handle_input(const std::string &input)
{
    std::vector<FWNetwork::Message<RemoteShell::TCPCustomMessageID>> messages;
    FWNetwork::Message<RemoteShell::TCPCustomMessageID> msg;

    if (input == "exit") {
        msg.header.id = RemoteShell::TCPCustomMessageID::CLIENT_DISCONNECTED;
        _running = false;
        std::cout << "Disconnecting client..." << std::endl;
        messages.push_back(msg);
        _network.sendMessagesToTCP(messages);
        return;
    } else if (input == "os") {
        std::cout << _os << std::endl;
        new_prompt();
        return;
    } else if (input == "host") {
        std::cout << _username << "@" << _host << ":" << _port <<  std::endl;
        new_prompt();
        return;
    } else if (input.substr(0, 3) == "cd " || input == "cd") { // To be updated soon (example: "ls -la; cd /; ls")
        send_command_to_TCP_server(input, RemoteShell::TCPCustomMessageID::CHANGE_DIRECTORY_IN);
        //new_prompt();
        return;
    }
    send_command_to_TCP_server(input, RemoteShell::TCPCustomMessageID::SHELL_CMD);
}

void ClientCore::ask_informations()
{
    std::vector<FWNetwork::Message<RemoteShell::TCPCustomMessageID>> messages;
    FWNetwork::Message<RemoteShell::TCPCustomMessageID> msg;

    msg.header.id = RemoteShell::TCPCustomMessageID::INFORMATION_ASK;
    messages.push_back(msg);
    _network.sendMessagesToTCP(messages);
}

void ClientCore::send_command_to_TCP_server(const std::string &input, enum RemoteShell::TCPCustomMessageID header)
{
    char cmd[MAX_CMD_LENGTH];
    std::string full_cmd = "cd " + _pwd.getCurrent() + "; " + input;
    std::vector<FWNetwork::Message<RemoteShell::TCPCustomMessageID>> messages;
    FWNetwork::Message<RemoteShell::TCPCustomMessageID> msg;

    msg.header.id = header;
    for (size_t i = 0; i < MAX_CMD_LENGTH; i++) {
        if (i >= full_cmd.length())
            cmd[i] = '\0';
        else
            cmd[i] = full_cmd[i];
    }
    cmd[MAX_CMD_LENGTH - 1] = '\0';
    std::cout << std::string(cmd) << std::endl;
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
                new_prompt();
                break;
            }
            case RemoteShell::TCPCustomMessageID::INFORMATION_SEND: {
                char os[MAX_OS_LENGTH];
                char pwd[MAX_PWD_LENGTH];
                char username[MAX_USERNAME_LENGTH];

                msg >> username;
                msg >> pwd;
                msg >> os;
                _os = std::string(os);
                _pwd.init(std::string(pwd));
                _username = std::string(username);
                new_prompt();
                break;
            }
            case RemoteShell::TCPCustomMessageID::CHANGE_DIRECTORY_OUT: {
                char pwd[MAX_PWD_LENGTH];
                char output[MAX_PWD_LENGTH];

                msg >> pwd;
                msg >> output;
                _pwd.change_directory(std::string(pwd));
                std::string output_str(output);
                if (output_str.length() > 0)
                    std::cout << output_str << std::endl;
                new_prompt();
                break;
            }
            default: {
                break;
            }
        }
    }
}

void ClientCore::new_prompt() const
{
    std::cout << _pwd.getCurrent() << " $> ";
}