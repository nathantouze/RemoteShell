#include "PWD.hpp"
#include "StringManagement.hpp"
#include <algorithm>

PWD::PWD() : _current("/"), _disk("")
{
}

PWD::PWD(const std::string &pwd) : _current(pwd)
{
    #ifdef _WIN32
        _disk = _current.substr(0, 2);
    #else
        _disk = "";
    #endif
}

PWD::~PWD()
{
}

void PWD::init(const std::string &pwd)
{
    _current = pwd;
    #ifdef _WIN32
        _disk = _current.substr(0, 2);
    #else
        _disk = "";
    #endif
}

const std::string PWD::getCurrent() const
{
    return _current;
}

const std::string PWD::getDisk() const
{
    return _disk;
}

std::vector<std::string> PWD::parsePath(std::string path) const
{
    std::vector<std::string> vPath = StringManagement::split(path, SEPARATOR);
    std::vector<std::string> vOpt = StringManagement::split(path, '/');

    if (vPath.size() > vOpt.size())
        return vPath;
    else
        return vOpt;
}

void PWD::setCurrent(const std::string &path)
{
    _current = path;
}

const std::string PWD::toString(const std::vector<std::string> &vPath) const
{
    std::string path_str = getDisk() + SEPARATOR;

    if (vPath.empty())
        return "";
    for (size_t i = 1; i < vPath.size(); i++)
        path_str += !vPath.at(i).empty() ? vPath.at(i) + SEPARATOR: "";
    if (!vPath.at(vPath.size() - 1).empty())
        path_str = path_str.substr(0, path_str.size() - 1);
    return path_str;
}

void PWD::change_directory(const std::string &path)
{
    std::vector<std::string> vCurrent = parsePath(getCurrent());
    std::vector<std::string> vPath = parsePath(path);

    if (vPath.at(0) == getDisk() || vPath.at(0) == "") {
        clearPath(vPath);
        setCurrent(toString(vPath));
        return;
    }
    for (auto folder: vPath)
        vCurrent.push_back(folder);
    clearPath(vCurrent);
    setCurrent(toString(vCurrent));
}

void PWD::clearPath(std::vector<std::string> &vPath)
{
    while (std::count(vPath.begin(), vPath.end(), "..") > 0 || std::count(vPath.begin(), vPath.end(), ".") > 0) {
        for (size_t i = 0; i < vPath.size(); i++) {
            if (vPath.at(i) == ".." && i > 1) {
                vPath.erase(vPath.begin() + i);
                vPath.erase(vPath.begin() + (i - 1));
                break;
            } else if (vPath.at(i) == ".." && i == 1) {
                vPath.erase(vPath.begin() + i);
                break;
            }
            if (vPath.at(i) == ".") {
                vPath.erase(vPath.begin() + i);
                break;
            }
        }
    }
}