#ifndef STRINGMANAGEMENT_HPP_
#define STRINGMANAGEMENT_HPP_

#include <string>
#include <vector>

class StringManagement {
    public:
        static char *string_to_char_array(const std::string &str, char *char_array, size_t size);
        static void replace_all(std::string &str, const std::string &before, const std::string &after);
        static std::vector<std::string> split(std::string str, char separator);
    protected:
    private:
};

#endif /* !STRINGMANAGEMENT_HPP_ */