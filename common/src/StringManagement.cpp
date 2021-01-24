#include "StringManagement.hpp"
#include <algorithm>

char *StringManagement::string_to_char_array(const std::string &str, char *char_array, size_t size)
{
    for (size_t i = 0; i < size; i++) {
        if (i > str.length())
            char_array[i] = '\0';
        else
            char_array[i] = str[i];
    }
    char_array[size - 1] = '\0';
    return char_array;
}

void StringManagement::replace_all(std::string &str, const std::string &before, const std::string &after)
{
    size_t start_pos = 0;

    while((start_pos = str.find(before, start_pos)) != std::string::npos) {
        str.replace(start_pos, before.length(), after);
        start_pos += after.length();
    }
}

std::vector<std::string> StringManagement::split(std::string string, char separator)
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