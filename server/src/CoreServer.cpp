#include "CoreServer.hpp"
#include "CustomDefines.hpp"
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
    std::cout << std::string(cmd) << std::endl;
    std::unique_ptr<FILE, decltype(&PCLOSE)> pipe(POPEN(cmd, "r"), PCLOSE);

    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
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
        username_str.substr(username_str.find_last_of("\\") != std::string::npos ? username_str.find_last_of("\\") + 1 : 0);
        return username_str.substr(0, username_str.size() - 1);
    #else
        std::string username_str = execute_shell_command("whoami");
        return username_str.substr(0, username_str.size() - 1);
    #endif
}

void CoreServer::send_informations_to_client(struct FWNetwork::OwnedMessageTCP<RemoteShell::TCPCustomMessageID> &msgGet)
{
    std::string os_str = get_os();
    std::string pwd_str = _pwd.getCurrent();
    std::string username_str = get_username();
    char os[MAX_OS_LENGTH];
    char pwd[MAX_PWD_LENGTH];
    char username[MAX_USERNAME_LENGTH];
    FWNetwork::Message<RemoteShell::TCPCustomMessageID> answer;

    for (unsigned int i = 0; i < MAX_OS_LENGTH; i++) {
        if (i >= MAX_OS_LENGTH)
            os[i] = '\0';
        else
            os[i] = os_str[i];
    }
    os_str[MAX_OS_LENGTH - 1] = '\0';
    for (unsigned int i = 0; i < MAX_PWD_LENGTH; i++) {
        if (i >= MAX_PWD_LENGTH)
            pwd[i] = '\0';
        else
            pwd[i] = pwd_str[i];
    }
    pwd_str[MAX_PWD_LENGTH - 1] = '\0';
    for (unsigned int i = 0; i < MAX_USERNAME_LENGTH; i++) {
        if (i >= MAX_USERNAME_LENGTH)
            username[i] = '\0';
        else
            username[i] = username_str[i];
    }
    username[MAX_USERNAME_LENGTH - 1] = '\0';
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

const std::vector<std::string> CoreServer::split_string(std::string string, char separator) const
{
    std::vector<std::string> vString = {};
    size_t count = std::count(string.begin(), string.end(), separator);

    for (size_t i = 0; i <= count; i++) {
        vString.push_back(string.substr(0, string.find_first_of(separator)));
        if (i == count)
            break;
        string = string.substr(string.find_first_of(separator) + 1);
    }
    return vString;
}

void CoreServer::replaceAll(std::string &str, const std::string &before, const std::string &after)
{
    size_t start_pos = 0;

    while((start_pos = str.find(before, start_pos)) != std::string::npos) {
        str.replace(start_pos, before.length(), after);
        start_pos += after.length();
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

    msgGet.msg >> cmd;
    output_str = execute_shell_command(cmd);
    cmd_splited = split_string(std::string(cmd), ';');
    if (output_str.length() == 0 && cmd_splited.at(1).length() > 3)
        _pwd.change_directory(cmd_splited.at(1).substr(3));
    std::string pwd_str = _pwd.getCurrent();
    for (size_t i = 0; i < MAX_PWD_LENGTH; i++) {
        if (i >= pwd_str.length())
            newPWD[i] = '\0';
        else
            newPWD[i] = pwd_str[i];
    }
    newPWD[MAX_PWD_LENGTH - 1] = '\0';
    for (size_t i = 0; i < MAX_OUTPUT_LENGTH; i++) {
        if (i >= output_str.length())
            output[i] = '\0';
        else
            output[i] = output_str[i];
    }
    output[MAX_OUTPUT_LENGTH - 1] = '\0';
    answer.header.id = RemoteShell::TCPCustomMessageID::CHANGE_DIRECTORY_OUT;
    answer << output;
    answer << newPWD;
    msgGet.remote->send(answer);
}