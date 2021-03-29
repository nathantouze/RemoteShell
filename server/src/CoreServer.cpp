#include "CoreServer.hpp"
#include "CustomDefines.hpp"
#include "StringManagement.hpp"
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/utsname.h>
#endif

CoreServer::CoreServer(int port) : _network(port), _fpsManager(30.0f), _running(true), _pwd()
{
    #ifdef _WIN32
        char pwd[MAX_PATH];
        GetCurrentDirectory(MAX_PATH, pwd);
        _pwd.init(std::string(pwd));
    #else
        std::string pwd_str = execute_shell_command("pwd");
        _pwd.init(pwd_str.substr(0, pwd_str.size() - 1));
    #endif
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
    while (fgets(buffer.data(), (int)buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

const std::string CoreServer::get_os()
{
    #ifdef _WIN32
        OSVERSIONINFOEX info;

        ZeroMemory(&info, sizeof(OSVERSIONINFOEX));
        info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
        GetVersionEx((LPOSVERSIONINFO)&info);
        int version = info.dwMajorVersion * 100 + info.dwMinorVersion * 10 - (info.wProductType != VER_NT_WORKSTATION ? 5 : 0);

        switch (version) {
            case 1000:
                return "Windows 10";
            case 630:
                return "Windows 8.1";
            case 620:
                return "Windows 8.0";
            case 615:
                return "Windows Server 2012";
            case 610:
                return "Windows 7";
            case 605:
                return "Windows Server 2008 R2";
            case 600:
                return "Windows Vista";
            default:
                return "Windows (unknown)";
        }
    #else
        struct utsname buf;
        uname(&buf);
        return std::string(buf.sysname) + " " + std::string(buf.version);
    #endif
}

const std::string CoreServer::get_username()
{
    #ifdef _WIN32
        std::string username_str = execute_shell_command("whoami");
        username_str = username_str.substr(username_str.find_last_of("\\") != std::string::npos ? username_str.find_last_of("\\") + 1 : 0);
        return username_str.substr(0, username_str.size() - 1);
    #else
        std::string username_str = execute_shell_command("whoami");
        return username_str.substr(0, username_str.size() - 1);
    #endif
}

void CoreServer::send_informations_to_client(struct FWNetwork::OwnedMessageTCP<RemoteShell::TCPCustomMessageID> &msgGet)
{
    char os[MAX_OS_LENGTH];
    char pwd[MAX_PWD_LENGTH];
    char username[MAX_USERNAME_LENGTH];
    FWNetwork::Message<RemoteShell::TCPCustomMessageID> answer;

    StringManagement::string_to_char_array(get_os(), os, MAX_OS_LENGTH);
    StringManagement::string_to_char_array(_pwd.getCurrent(), pwd, MAX_PWD_LENGTH);
    StringManagement::string_to_char_array(get_username(), username, MAX_USERNAME_LENGTH);

    answer.header.id = RemoteShell::TCPCustomMessageID::INFORMATION_SEND;
    answer << os << pwd << username;
    msgGet.remote->send(answer);
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
                StringManagement::string_to_char_array(output_str, output, MAX_OUTPUT_LENGTH);
                answer.header.id = RemoteShell::TCPCustomMessageID::SHELL_OUTPUT;
                answer << output;
                msgGet.remote->send(answer);
                break;
            }
            case RemoteShell::TCPCustomMessageID::INFORMATION_ASK: {
                send_informations_to_client(msgGet);
                break;
            }
            case RemoteShell::TCPCustomMessageID::CHANGE_DIRECTORY_IN: {
                change_directory(msgGet);
            }
            default: {
                break;
            }
        }
    }
}

void CoreServer::change_directory(FWNetwork::OwnedMessageTCP<RemoteShell::TCPCustomMessageID> &msgGet)
{
    char cmd[MAX_CMD_LENGTH];
    char output[MAX_OUTPUT_LENGTH];
    char newPWD[MAX_PWD_LENGTH];
    FWNetwork::Message<RemoteShell::TCPCustomMessageID> answer;
    std::string output_str;
    std::vector<std::string> cmd_splited;
    std::string cmd_cleared;

    msgGet.msg >> cmd;
    output_str = execute_shell_command(cmd);
    cmd_cleared = std::string(cmd);
    StringManagement::replace_all(cmd_cleared, "; ", ";");
    cmd_splited = StringManagement::split(cmd_cleared, ';');

    if (output_str.length() == 0 && cmd_splited.at(1).length() > 3)
        _pwd.change_directory(cmd_splited.at(1).substr(3));

    StringManagement::string_to_char_array(_pwd.getCurrent(), newPWD, MAX_PWD_LENGTH);
    StringManagement::string_to_char_array(output_str, output, MAX_OUTPUT_LENGTH);
    answer.header.id = RemoteShell::TCPCustomMessageID::CHANGE_DIRECTORY_OUT;
    answer << output;
    answer << newPWD;
    msgGet.remote->send(answer);
}