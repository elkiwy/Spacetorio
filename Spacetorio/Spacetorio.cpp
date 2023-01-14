// Spacetorio.cpp : Defines the entry point for the application.
//

//Using SDL and standard IO
#include <SDL.h>
#include <iostream>
#include <ostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#include "GameController.hpp"
#include "SDL_keyboard.h"
#include "SDL_video.h"
#include "Utils_time.hpp"

#include "SDL_events.h"
#include "SDL_render.h"
#include "SDL_stdinc.h"
#include "SDL_ttf.h"

#include "backends/imgui_impl_sdl.h"

float global_avgFPS = 0;
Renderer* global_renderer = nullptr;

int main(int argc, char* args[]) {
    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    //Create window
    const int TARGET_FPS = 60;
    const int TARGET_TICKS_PER_FRAME = 1000/TARGET_FPS;
    iSize screenRes = {1280, 720};
    SDL_WindowFlags wf = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
    SDL_Window* window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenRes.w, screenRes.h, wf);
    if (window == NULL){
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() == -1){
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        return 1;
    }


    //Update the random seed
    srand(time(NULL));

    {
        //Initialize GameController
        GameController gc = GameController(window, screenRes);
        global_renderer = &gc.getRenderer();
        gc.init();


        ImGuiIO& io = ImGui::GetIO(); (void)io;

        //Hack to get window to stay up
        Uint32 startTime = 0;
        bool quit = false;
        Timer capTimer;
        int countedFrames = 0;
        Timer fpsTimer; fpsTimer.start();
        while (quit == false) {
            capTimer.start();
            //Process all the event queue
            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                ImGui_ImplSDL2_ProcessEvent(&e);
                if (e.type == SDL_QUIT) {
                    quit = true;
                    gc.quit();
                }else if(e.type == SDL_KEYDOWN && e.key.repeat == 0){
                    gc.onKeyPressed(e.key.keysym.sym);
                }else if(e.type == SDL_MOUSEWHEEL){
                    gc.onMouseWheel(e.wheel.y);
                }
            }

            //Calculate avg fps so far
            float avgFPS = countedFrames / (fpsTimer.getTicks() / 1000.f);
            if (avgFPS > 2000000){avgFPS = 0;}
            global_avgFPS = avgFPS;

            //Update logic
            const Uint8* keyState = SDL_GetKeyboardState(NULL);
            gc.update(keyState);

            //Render updates on screen
            gc.renderBegin();
            gc.render();
            gc.renderGUI();
            gc.renderEnd();

            //Update frame counter
            ++countedFrames;

            // If frame finished early
            int frameTicks = capTimer.getTicks();
            if (frameTicks < TARGET_TICKS_PER_FRAME) {
                // Wait remaining time
                SDL_Delay(TARGET_TICKS_PER_FRAME - frameTicks);
            }
        }
    }

    //Destroy window
    SDL_DestroyWindow(window);

    //Quit SDL subsystems
    TTF_Quit();
    SDL_Quit();

    return 0;
}
