
#include "FPSManager.hpp"

FPSManager::FPSManager(float fpsWanted): _fpsWanted(fpsWanted)
{
    
};

void FPSManager::start()
{
    _begin = std::chrono::system_clock::now();
};

bool FPSManager::isTimePassed()
{
    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();

    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now - _begin);

    if ((double)(milliseconds.count()) > (double)(1000 / _fpsWanted)) {
        _begin = std::chrono::system_clock::now();
        return true;
    }
    return false;
};
