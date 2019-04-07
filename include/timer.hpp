#pragma once

#include <chrono>

class Timer {
    public:
        void init();
        void reset();
        float getTime();

    private:
        void draw();
        std::chrono::high_resolution_clock::time_point timer;
};
