#ifndef UTILS_TIME_H_
#define UTILS_TIME_H_

#include <iostream>
#include <chrono>

#include "SDL_stdinc.h"
#include "SDL_timer.h"
class Timer {
    public:
        Timer() {
            paused = false;
            started = false;
            startTicks = 0;
            pausedTicks = 0;
        }
        virtual ~Timer() = default;

        void start(){
            started = true;
            paused = false;
            startTicks = SDL_GetTicks();
            pausedTicks = 0;
        }

        void stop(){
            started = false;
            paused = false;
            startTicks = 0;
            pausedTicks = 0;
        }

        void pause(){
            if(started && !paused){
                paused = true;
                pausedTicks = SDL_GetTicks() - startTicks;
                startTicks = 0;
            }
        }

        void unpause(){
            if(started && paused){
                paused = false;
                startTicks = SDL_GetTicks() - pausedTicks;
                pausedTicks = 0;
            }
        }

        Uint32 getTicks(){
            if(started){return (paused) ? pausedTicks : SDL_GetTicks() - startTicks;
            }else{return 0;}
        }

        bool isStarted(){
            return started;
        }

        bool isPaused(){
            return paused && started;
        }

    private:
        Uint32 startTicks;
        Uint32 pausedTicks;
        bool paused;
        bool started;
};

class HighResTimer{
    typedef std::chrono::high_resolution_clock Clock;
    std::chrono::time_point<Clock> start;

    public:
        HighResTimer(){
            start = Clock::now();
        }

        long long timePassed(){
            auto now = Clock::now();
            return std::chrono::duration_cast<std::chrono::nanoseconds>(now - start).count();
        }
};


#endif // UTILS_TIME_H_
