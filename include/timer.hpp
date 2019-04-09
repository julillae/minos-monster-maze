#pragma once

#include <chrono>

class Timer {
    public:
        void init();
        void reset();
        float getTime();
        void recordSavedTime(float st);
        void recordPausedTime(float start, float end);
        void addCumulativeTime(float toAdd);
        void resetCumulativeTime();
        void resetPausedTime();
        void cleanSlate();

    private:
        void draw();
        std::chrono::high_resolution_clock::time_point timer;
        float savedTime;
        float pausedTime;
        float cumulativeTime;
};
