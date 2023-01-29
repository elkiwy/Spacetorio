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
#include "Components_colliders.hpp"
#include "Components_clickables.hpp"

#include "Entity_player.hpp"


Scene::Scene(){}

Scene::~Scene(){

}

void Scene::init(){
    registerGenericComponent<RenderableComponent, RenderableCircleComponent>();
    registerGenericComponent<RenderableComponent, RenderableRectComponent>();
    registerGenericComponent<RenderableComponent, PlanetComponent>();
    registerGenericComponent<RenderableComponent, ColliderCircleComponent>();
    registerGenericComponent<RenderableComponent, ColliderRectangleComponent>();
    registerGenericComponent<RenderableComponent, ColliderLineComponent>();
    registerGenericComponent<RenderableComponent, ClickableCircleComponent>();
    registerGenericComponent<RenderableComponent, ClickableRectangleComponent>();

    registerGenericComponent<UpdatableComponent, DynamicPositionComponent>();
    registerGenericComponent<UpdatableComponent, PlayerSpaceshipComponent>();
    registerGenericComponent<UpdatableComponent, PlayerComponent>();

    registerGenericComponent<ColliderComponent, ColliderCircleComponent>();
    registerGenericComponent<ColliderComponent, ColliderRectangleComponent>();
    registerGenericComponent<ColliderComponent, ColliderLineComponent>();

    registerGenericComponent<ClickableComponent, ClickableCircleComponent>();
    registerGenericComponent<ClickableComponent, ClickableRectangleComponent>();

    registerMonoGenericComponent<PositionComponent, DynamicPositionComponent>();
    registerMonoGenericComponent<PositionComponent, StaticPositionComponent>();



    cam.init();
}

void Scene::render(){
    //Render all the renderable entities
    const ShapeRectangle& cameraRect = cam.getCameraShape();
    auto view = registry.view<RenderableComponent, PositionComponent>();
    //std::cout << "---------" << std::endl;
    for(auto entity: view){
        //std::cout << "Rendering entity: " << (int)entity << std::endl;
        PositionComponent* pos = static_cast<PositionComponent*>(view.get<PositionComponent>(entity).impl);
        RenderableComponent& renderable = view.get<RenderableComponent>(entity);
        for(auto impl: renderable.impls){
            if (impl == nullptr){break;}
            //std::cout << " -- Rendering comp: " << (void*)impl << std::endl;
            bool inCamera = cameraRect.checkCollisionWithPoint(ShapePoint(pos->pos));
            if (inCamera){
                static_cast<RenderableComponent*>(impl)->render(*pos, cam);
            }
        }
    }

    //Draw camera crosshair and coordinates
    renderCameraCrosshair();
}

void Scene::renderCameraCrosshair(){
    int len = 10;
    //iPoint c = iPoint(((int)cam.screen_size.w)/2, ((int)cam.screen_size.h)/2);
    //global_renderer->drawLine(c.x, c.y-len, c.x, c.y+len, {0,0,0,255});
    //global_renderer->drawLine(c.x-len, c.y, c.x+len, c.y, {0,0,0,255});
    //fPoint worldPos = cam.screenToWorld({(float)c.x, (float)c.y});
    //global_renderer->drawText(c.x, c.y,    std::to_string(worldPos.x), {0,0,0,255});
    //global_renderer->drawText(c.x, c.y+16, std::to_string(worldPos.y), {0,0,0,255});
}

void Scene::renderGUI(){

    //
    if (false){
        ImGui::Begin("Active Scene GUI");
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

                //if (registry.any_of<PositionComponent>(e)){
                //    auto& pos = registry.get<PositionComponent>(e);
                //    ImGui::Text("Position:");
                //    ImGui::SliderFloat("Pos X:", &pos.pos.x, -900.0f, 900.0f);
                //    ImGui::SliderFloat("Pos Y:", &pos.pos.y, -900.0f, 900.0f);
                //}

                if (registry.any_of<RenderableComponent>(e)){
                    ImGui::Text("RenderableComponent : YES");
                }

                if (registry.any_of<ClickableComponent>(e)){
                    ImGui::Text("ClickableComponent : YES");
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
    }

    //Camera
    cam.renderGUI();
}


void Scene::update(const Uint8* ks){
    //Process inputs
    if (ks[SDL_SCANCODE_RIGHT] || ks[SDL_SCANCODE_LEFT] || ks[SDL_SCANCODE_DOWN] || ks[SDL_SCANCODE_UP]) {
        float camSpeedInc = 2.0f;
        float camSpdDx = 0.0f;
        float camSpdDy = 0.0f;
        if (ks[SDL_SCANCODE_RIGHT]) camSpdDx += camSpeedInc;
        if (ks[SDL_SCANCODE_LEFT])  camSpdDx -= camSpeedInc;
        if (ks[SDL_SCANCODE_UP])    camSpdDy += camSpeedInc;
        if (ks[SDL_SCANCODE_DOWN])  camSpdDy -= camSpeedInc;
        cam.addSpd(camSpdDx, camSpdDy);
    }

    //Cycle all the active colliders
    auto collidersView = registry.view<ColliderComponent>();
    for(auto& entity: collidersView){
        auto& collider = collidersView.get<ColliderComponent>(entity);
        if (collider.active == false) { continue; }
        for(auto impl: collider.impls){
            if (impl == nullptr){break;}

            //Get all the others colliders (TODO: Probably could be optimized)
            auto others = registry.view<ColliderComponent>();
            for (auto& other : others) {
                if (other == entity){continue;} //Skip myself

                //Get all the impls of the other collider
                ColliderComponent& otherCollider = others.get<ColliderComponent>(other);
                for(auto otherImpl: otherCollider.impls){
                    if (otherImpl == nullptr){break;}

                    //Check the active entity against all the impls of the other collider
                    ColliderComponent* implCollider = static_cast<ColliderComponent*>(impl);
                    ColliderComponent* otherImplCollider = static_cast<ColliderComponent*>(otherImpl);
                    implCollider->debug_color = {0,255,0,255};
                    otherImplCollider->debug_color = {0,255,0,255};
                    bool check = implCollider->checkCollisions(otherImplCollider);
                    if (check){
                        implCollider->debug_color = {255,0,0,255};
                        otherImplCollider->debug_color = {255,0,0,255};
                    }

                    //std::cout << "Collision "<<check<<" between " << (void*)impl << " and " << (void*)otherImpl << std::endl;
                }
            }
        }
    }

    //Check for hovered clickables
    iPoint mousePos = {0,0};
    SDL_GetMouseState(&mousePos.x, &mousePos.y);
    fPoint worldMouse = cam.screenToWorld(fPoint(mousePos.x, mousePos.y));
    ShapePoint worldMousePt = ShapePoint(worldMouse);
    auto viewClickables = registry.view<ClickableComponent>();
    for(auto entity: viewClickables){
        auto& clickable = viewClickables.get<ClickableComponent>(entity);
        if (clickable.active == false){continue;}
        for(auto impl: clickable.impls){
            if (impl == nullptr){break;}
            ClickableComponent* implCasted = static_cast<ClickableComponent*>(impl);
            implCasted->checkHovered(worldMousePt);
        }
    }


    //Update all the updatable entities
    auto view = registry.view<UpdatableComponent>();
    for(auto entity: view){
        auto& updatable = view.get<UpdatableComponent>(entity);
        Entity e = {entity, this};
        for(auto impl: updatable.impls){
            if (impl == nullptr){break;}
    //        static_cast<UpdatableComponent*>(impl)->update(e, ks);
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


void Scene::onMouseLeftClick(){
    //Check for hovered clickables
    iPoint mousePos = {0,0};
    SDL_GetMouseState(&mousePos.x, &mousePos.y);
    fPoint worldMouse = cam.screenToWorld(fPoint(mousePos.x, mousePos.y));
    ShapePoint worldMousePt = ShapePoint(worldMouse);
    auto viewClickables = registry.view<ClickableComponent>();
    for(auto entity: viewClickables){
        auto& clickable = viewClickables.get<ClickableComponent>(entity);
        if (clickable.active == false){continue;}
        for(auto impl: clickable.impls){
            if (impl == nullptr){break;}
            ClickableComponent* implCasted = static_cast<ClickableComponent*>(impl);
            bool hovered = implCasted->checkHovered(worldMousePt);
            if (hovered){ implCasted->click(); }
        }
    }


    std::cout << "World pos " << worldMouse << std::endl;
}


void Scene::onKeyPressed(SDL_Keycode key){
    //Update all the updatable entities
    auto view = registry.view<UpdatableComponent>();
    for(auto entity: view){
        auto& updatable = view.get<UpdatableComponent>(entity);
        Entity e = {entity, this};
        for(auto impl: updatable.impls){
            if (impl == nullptr){break;}
            static_cast<UpdatableComponent*>(impl)->onKeyPressed(key);
        }
    }
}
