#ifndef GAMECONTROLLER_H_
#define GAMECONTROLLER_H_

#include "SDL_keycode.h"

#include "Renderer.hpp"
#include "SDL_stdinc.h"
#include "Universe.hpp"

class GameController {
    public:
        GameController(SDL_Window* sdlWindow, iSize screenRes);
        virtual ~GameController();
        Renderer& getRenderer() { return renderer; }

        //Game events
        void init();
        void update(const Uint8* keyState);
        void renderBegin();
        void render();
        void renderGUI();
        void renderEnd();
        void quit();

        //User events
        void onKeyPressed(SDL_Keycode key);
        void onMouseWheel(float dy);


    private:
        SDL_Window* sdlWindow;
        Renderer renderer;
        Universe universe;
};


#endif // GAMECONTROLLER_H_
