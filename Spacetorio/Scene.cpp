#include "Scene.hpp"
#include "Entity.hpp"
#include "Renderer.hpp"
#include <iostream>
#include <ostream>
#include <string>

#include "SDL_keycode.h"
#include "SDL_mouse.h"
#include "SDL_render.h"
#include "imgui.h"

#include "Universe.hpp"
#include "Utils_math.hpp"

Scene::Scene(){

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

    //Draw camera crosshair and coordinates
    int len = 10;
    iPoint c = {640, 360};
    global_renderer->drawLine(c.x, c.y-len, c.x, c.y+len, {0,0,0,255});
    global_renderer->drawLine(c.x-len, c.y, c.x+len, c.y, {0,0,0,255});
    fPoint worldPos = cam.screenToWorld({(float)c.x, (float)c.y});
    global_renderer->drawText(c.x, c.y,    std::to_string(worldPos.x), {0,0,0,255});
    global_renderer->drawText(c.x, c.y+16, std::to_string(worldPos.y), {0,0,0,255});
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
