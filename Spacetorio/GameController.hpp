#ifndef GAMECONTROLLER_H_
#define GAMECONTROLLER_H_

#include "SDL_keycode.h"

#include "Renderer.hpp"
#include "Universe.hpp"

class GameController {
    public:
        GameController(SDL_Window* sdlWindow, iSize screenRes);
        virtual ~GameController();
        Renderer& getRenderer() { return renderer; }

        //Game events
        void update();
        void renderBegin();
        void render();
        void renderGUI();
        void renderEnd();
        void quit();

        //User events
        void onKeyDown(SDL_Keycode key);


    private:
        SDL_Window* sdlWindow;
        Renderer renderer;
        Universe universe;
};


#endif // GAMECONTROLLER_H_
