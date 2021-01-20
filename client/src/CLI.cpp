#include "CLI.hpp"

CLI::CLI()
{
    input = "";
    sendOverNextLine = true;
    continueGettingInput = true;

    std::thread([&]()
    {
        std::string synchronousInput;
        char nextCharacter;

        while (continueGettingInput && input != "exit") {
            synchronousInput = "";
            while (continueGettingInput) {
                while (std::cin.peek() == EOF)
                    std::this_thread::yield();
                nextCharacter = std::cin.get();
                if (nextCharacter == '\n')
                    break;
                synchronousInput += nextCharacter;
            }
            if (!continueGettingInput)
                break;
            while (continueGettingInput && !sendOverNextLine)
                std::this_thread::yield();
            if (!continueGettingInput)
                break;
            inputLock.lock();
            input = synchronousInput;
            inputLock.unlock();
            sendOverNextLine = false;
        }
    }).detach();
}

CLI::~CLI()
{
    continueGettingInput = false;
}

std::string CLI::getline() 
{
    if (sendOverNextLine) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        return "";
    } else {
        inputLock.lock();
        std::string returnInput = input;
        inputLock.unlock();
        sendOverNextLine = true;
        return returnInput;
    }
}