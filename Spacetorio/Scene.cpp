#include "Scene.hpp"
#include "Entity.hpp"
#include <iostream>
#include <ostream>
#include <string>

#include "SDL_keycode.h"
#include "SDL_mouse.h"
#include "imgui.h"

#include "Planet.hpp"
#include "Utils_math.hpp"

Scene::Scene(){
    for(int i=0;i<10;i++){
        fPoint pos = {
            randFloat(300.0f, 700.0f),
            randFloat(300.0f, 700.0f)
        };
        float size = randFloat(50.0f, 100.0f);
        Planet(registry, this, "Earth "+std::to_string(i), pos, size);
    }


    Planet(registry, this, "test origin", {0.0f, 0.0f}, 10.0f);
    Planet(registry, this, "test center", {640.0f, 360.0f}, 20.0f);
}

Scene::~Scene(){

}

void Scene::update(){
    cam.update();
}

void Scene::render() const{
    auto view = registry.view<PositionComponent,RenderableCircleComponent>();
    for(auto entity: view){
        auto [pos, rc] = view.get(entity);
        rc.render(pos, cam);
    }
}

void Scene::renderGUI(){
    ImGui::Begin("Active Scene GUI");

    ImGui::Text("Entities");
    ImGui::BeginChild("Scrolling");
    auto view = registry.view<TagComponent, PositionComponent>();
    for(auto entity: view){
        auto [tag, pos] = view.get<TagComponent, PositionComponent>(entity);
        if (ImGui::TreeNode(tag.tag.c_str())){
            ImGui::Text("Position:");
            ImGui::TreePop();
        }
    }
    ImGui::EndChild();
    ImGui::End();



    ImGui::Begin("Camera");
    ImGui::Text("Camera");
    ImGui::SliderFloat("Cam X:", &cam.pos.x, -900.0f, 900.0f);
    ImGui::SliderFloat("Cam Y:", &cam.pos.y, -900.0f, 900.0f);
    ImGui::SliderFloat("Cam Zoom:", &cam.zoom, 0.0f, 3.0f);
    ImGui::End();
}


void Scene::onKeyDown(const Uint8* ks){
    if (ks[SDL_SCANCODE_RIGHT] || ks[SDL_SCANCODE_LEFT] || ks[SDL_SCANCODE_DOWN] || ks[SDL_SCANCODE_UP]) {
        float camSpeedInc = 0.5f;
        float camSpdDx = 0.0f;
        float camSpdDy = 0.0f;
        if (ks[SDL_SCANCODE_RIGHT]) camSpdDx += camSpeedInc;
        if (ks[SDL_SCANCODE_LEFT]) camSpdDx -= camSpeedInc;
        if (ks[SDL_SCANCODE_DOWN]) camSpdDy += camSpeedInc;
        if (ks[SDL_SCANCODE_UP]) camSpdDy -= camSpeedInc;
        cam.addSpd(camSpdDx, camSpdDy);
    }
}


void Scene::onMouseWheel(float dy){
    const float zoomFactor = 0.1f;
    iPoint mousePos = {0,0};
    SDL_GetMouseState(&mousePos.x, &mousePos.y);
    cam.zoomBy(dy*zoomFactor, {(float)mousePos.x, (float)mousePos.y});
    //cam.testZoom(dy*zoomFactor);
}
