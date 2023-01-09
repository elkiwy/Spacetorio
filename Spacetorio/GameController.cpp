#include "GameController.hpp"
#include "SDL.h"
#include "SDL_video.h"
#include <iostream>


GameController::GameController(SDL_Window* sdlWin, iSize screenRes) : renderer(sdlWin, screenRes){
    sdlWindow = sdlWin;
}

GameController::~GameController(){

}

void GameController::update(){
    Scene& activeScene = universe.getCurrentScene();
    activeScene.update();
}

void GameController::renderBegin(){
    renderer.renderFrameBegin();
}

void GameController::render(){
    const Scene& sceneToRender = universe.getCurrentScene();
    renderer.renderScene(sceneToRender);
}

void GameController::renderGUI(){
    Scene& activeScene = universe.getCurrentScene();
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
}

void GameController::onKeyDown(const Uint8* keyState){
    Scene& activeScene = universe.getCurrentScene();
    activeScene.onKeyDown(keyState);
}

void GameController::onMouseWheel(float dy){
    std::cout << "Mouse wheel: " << dy << std::endl;
    Scene& activeScene = universe.getCurrentScene();
    activeScene.onMouseWheel(dy);
}
