#include "Scene.hpp"
#include "Entity.hpp"
#include <string>

#include "imgui.h"

Scene::Scene(){
    for(int i=0;i<10;i++){
        createEntity("coso " + std::to_string(i));
    }

}

Scene::~Scene(){

}

Entity Scene::createEntity(const std::string& name){
    Entity e = {registry.create(), this};
    e.addComponent<TagComponent>((name.empty()) ? "_UnnamedEntity_" : name);
    e.addComponent<PositionComponent>(0.0, 0.0);
    return e;
}

void Scene::render() const{

}

void Scene::renderGUI() const{
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
}
