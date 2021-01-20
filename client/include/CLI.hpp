#ifndef CLI_HPP_
#define CLI_HPP_

#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>

class CLI {
    public:
        CLI();
        ~CLI();
        std::string getline();
    protected:
    private:
        std::atomic<bool> continueGettingInput;
        std::atomic<bool> sendOverNextLine;
        std::mutex inputLock;
        std::string input;
};

#endif /* !CLI_HPP_ */
