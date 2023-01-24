#include "GameController.hpp"
#include "SDL.h"
#include "SDL_keycode.h"
#include "SDL_video.h"
#include <iostream>


GameController::GameController(SDL_Window* sdlWin, iSize screenRes) : renderer(sdlWin, screenRes){
    sdlWindow = sdlWin;
}

GameController::~GameController(){

}

void GameController::init(){
    universe.init();
}

void GameController::update(const Uint8* keyState){
    universe.update(keyState);
}

void GameController::renderBegin(){
    renderer.renderFrameBegin();
}

void GameController::render(){
    Scene* sceneToRender = universe.getCurrentScene();
    renderer.renderScene(sceneToRender);
}

void GameController::renderGUI(){
    Scene* activeScene = universe.getCurrentScene();
    renderer.renderGUI(activeScene);
}

void GameController::renderEnd(){
    renderer.renderFrameEnd();
}

void GameController::quit(){
    std::cout << "Quitting... " << std::endl;
}

void GameController::onKeyPressed(SDL_Keycode key){
    std::cout << "Key down: " << key << std::endl;
    if (key == SDLK_y){
        //Switch scene
        std::cout << "Biome scene: " << (void*)universe.getBiomeScene() << std::endl;
        std::cout << "Space scene: " << (void*)universe.getSpaceScene() << std::endl;
        std::cout << "Active scene: " << (void*)universe.getCurrentScene() << std::endl;
        std::cout << "check: " << (universe.getCurrentScene() == universe.getBiomeScene()) << std::endl;

        Scene* s = (universe.getCurrentScene() == universe.getBiomeScene()) ? universe.getSpaceScene() : universe.getBiomeScene();
        universe.switchScene(s);
    }

    Scene* activeScene = universe.getCurrentScene();
    activeScene->onKeyPressed(key);
}

void GameController::onMouseWheel(float dy){
    Scene* activeScene = universe.getCurrentScene();
    activeScene->onMouseWheel(dy);
}

void GameController::onMouseLeftClick(){
    Scene* activeScene = universe.getCurrentScene();
    activeScene->onMouseLeftClick();
}
