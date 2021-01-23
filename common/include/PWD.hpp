#ifndef PWD_HPP_
#define PWD_HPP_

#include <vector>
#include <string>
#include "CustomDefines.hpp"

class PWD {
    public:
        PWD();
        PWD(const std::string &pwd);
        ~PWD();
        void init(const std::string &pwd);
        const std::string getCurrent() const;
        const std::string getDisk() const;
        std::vector<std::string> parsePath(std::string path) const;
        void change_directory(const std::string &path);
    protected:
    private:
        void setCurrent(const std::string &path);
        void clearPath(std::vector<std::string> &vPath);
        const std::string toString(const std::vector<std::string> &vPath) const;
        std::string _current;
        std::string _disk;
};

#endif /* !PWD_HPP_ */
