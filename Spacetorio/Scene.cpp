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

#include "Components.hpp"
#include "Components_updatable.hpp"
#include "Components_position.hpp"
#include "Components_renderables.hpp"
#include "Components_planet.hpp"


Scene::Scene(){
    registerGenericComponent<RenderableComponent, RenderableCircleComponent>();
    registerGenericComponent<RenderableComponent, RenderableRectComponent>();
    registerGenericComponent<RenderableComponent, PlanetComponent>();

    registerGenericComponent<UpdatableComponent, PositionComponent>();
    registerGenericComponent<UpdatableComponent, PlayerSpaceshipComponent>();
}


Scene::~Scene(){

}

void Scene::render(){
    //Render all the renderable entities
    auto view = registry.view<PositionComponent,RenderableComponent>();
    for(auto entity: view){
        auto& pos = view.get<PositionComponent>(entity);
        auto& renderable = view.get<RenderableComponent>(entity);
        for(auto impl: renderable.impls){
            if (impl == nullptr){break;}
            ((RenderableComponent*)impl)->render(pos, cam);
        }
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

    //
    ImGui::Text("Entities");
    ImGui::BeginChild("Scrolling");
    auto view = registry.view<TagComponent>();
    for(auto e: view){
        auto& tag  = view.get<TagComponent>(e);
        std::string name = tag.tag + "("+std::to_string((int)e)+")";
        if (ImGui::TreeNode(name.c_str())){

            if (registry.any_of<UpdatableComponent>(e)){
                ImGui::Text("UpdatableComponent : YES");
            }

            if (registry.any_of<PositionComponent>(e)){
                auto& pos = registry.get<PositionComponent>(e);
                ImGui::Text("Position:");
                ImGui::SliderFloat("Pos X:", &pos.pos.x, -900.0f, 900.0f);
                ImGui::SliderFloat("Pos Y:", &pos.pos.y, -900.0f, 900.0f);
                ImGui::SliderFloat("Speed X:", &pos.spd.x, -10.0f, 10.0f);
                ImGui::SliderFloat("Speed Y:", &pos.spd.y, -10.0f, 10.0f);
            }

            if (registry.any_of<RenderableComponent>(e)){
                ImGui::Text("RenderableComponent : YES");
            }

            if (registry.any_of<RenderableCircleComponent>(e)){
                auto& rc = registry.get<RenderableCircleComponent>(e);
                ImGui::Text("Renderable Circle:");
                ImGui::SliderFloat("Size", &rc.s, -900.0f, 900.0f);
            }


            ImGui::TreePop();
        }
    }
    ImGui::EndChild();
    ImGui::End();

    //Camera
    ImGui::Begin("Camera");
    ImGui::Text("Camera");
    ImGui::SliderFloat("Cam X:", &cam.pos.x, -900.0f, 900.0f);
    ImGui::SliderFloat("Cam Y:", &cam.pos.y, -900.0f, 900.0f);
    ImGui::SliderFloat("Cam Zoom:", &cam.zoom, 0.0f, 3.0f);
    ImGui::End();
}


void Scene::update(const Uint8* ks){
    //Process inputs
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

    //Update all the updatable entities
    auto view = registry.view<UpdatableComponent, PositionComponent>();
    for(auto entity: view){
        auto& updatable = view.get<UpdatableComponent>(entity);
        Entity e = {entity, this};
        for(auto impl: updatable.impls){
            if (impl == nullptr){break;}
            ((UpdatableComponent*)impl)->update(e, ks);
        }
    }

    //Update camera
    cam.update(ks);
}


void Scene::onMouseWheel(float dy){
    const float zoomFactor = 0.1f;
    iPoint mousePos = {0,0};
    SDL_GetMouseState(&mousePos.x, &mousePos.y);
    cam.zoomBy(dy*zoomFactor, {(float)mousePos.x, (float)mousePos.y});
    //cam.testZoom(dy*zoomFactor);
}
