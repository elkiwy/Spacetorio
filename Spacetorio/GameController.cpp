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

}

void GameController::renderBegin(){
    renderer.renderFrameBegin();
}

void GameController::render(){
    const Scene& sceneToRender = universe.getCurrentScene();
    renderer.renderScene(sceneToRender);
}

void GameController::renderGUI(){
    const Scene& activeScene = universe.getCurrentScene();
    renderer.renderGUI(activeScene);
}

void GameController::renderEnd(){
    renderer.renderFrameEnd();
}

void GameController::quit(){
    std::cout << "Quitting... " << std::endl;
}

void GameController::onKeyDown(SDL_Keycode key){
    std::cout << "Key down: " << key << std::endl;
}
