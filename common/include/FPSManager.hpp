
#ifndef FPSMANAGERRTYPE_HPP
#define FPSMANAGERRTYPE_HPP

#include <chrono>

class FPSManager {
    public:
        FPSManager(float fpsWanted);
        ~FPSManager() = default;
        void start();
        bool isTimePassed();

    private:
        std::chrono::time_point<std::chrono::system_clock> _begin;
        float _fpsWanted;
};

#endif