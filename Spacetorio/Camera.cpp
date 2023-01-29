#include "Camera.hpp"
#include "Components_position.hpp"
#include "Texture.hpp"
#include "Utils_points.hpp"
#include "Renderer.hpp"
#include <iostream>

#include <glm/ext/matrix_transform.hpp>
#include <string>

Camera::Camera() {}
Camera::~Camera() {}

void Camera::init(){
    std::cout << "Initializing camera" << std::endl;
    if (global_renderer != nullptr){
        iSize sz = global_renderer->getScreenSize();
        screen_size = fSize(sz.w, sz.h);
        moveTo(0, 0);
    }
    std::cout << "done camera" << std::endl;
}

void Camera::update(const Uint8 *ks) {
    if (this->target == nullptr){
        //Free moving
        spd.dampVector(0.95f);
        moveBy(spd.x, spd.y);
    }else{
        //Locked to target
        this->moveTo(target->pos.x, target->pos.y);
    }
}

void Camera::renderGUI(){
    ImGui::Begin("Camera");
    ImGui::Text("Camera");
    ImGui::SliderFloat("Cam X:", &pos.x, -9000.0f, 9000.0f);
    ImGui::SliderFloat("Cam Y:", &pos.y, -9000.0f, 9000.0f);
    ImGui::SliderFloat("Cam Zoom:", &zoom, 0.0f, 3.0f);

    ImGui::Separator();
    ImGui::Text("Mouse World Positon:");
    iPoint mousePos = {0,0};
    SDL_GetMouseState(&mousePos.x, &mousePos.y);
    fPoint worldMouse = screenToWorld(fPoint(mousePos.x, mousePos.y));
    ImGui::Text("X: %s", std::to_string((int)worldMouse.x).c_str());
    ImGui::Text("Y: %s", std::to_string((int)worldMouse.y).c_str());

    ImGui::End();
}


fPoint Camera::getCameraWorldCenter() const{
    return {
        pos.x + (screen_size.w * 0.5f)/zoom,
        pos.y + (screen_size.h * 0.5f)/zoom
    };
}

void Camera::moveTo(float x, float y) {
    pos.x = x - (screen_size.w * 0.5f)/zoom;
    pos.y = y - (screen_size.h * 0.5f)/zoom;
    this->updateCameraMatrix();
}

void Camera::moveBy(float dx, float dy) {
    pos.x += dx;
    pos.y += dy;
    this->updateCameraMatrix();
}

void Camera::addSpd(float dx, float dy) {
    spd.x += dx;
    spd.y += dy;
}

void Camera::zoomBy(float dy, fPoint screenRef) {
    fPoint worldRef = screenToWorld(screenRef);
    zoom += dy;
    fPoint movedScreenRef = worldToScreen(worldRef);
    fVec diff = screenRef - movedScreenRef;
    moveBy(-diff.x / zoom, diff.y / zoom);
}

fPoint Camera::screenToWorld(fPoint screenPos) const {
    return {(screenPos.x / zoom) + pos.x, ((screen_size.h - screenPos.y) / zoom) + pos.y};
}

fPoint Camera::worldToScreen(fPoint worldPos) const {
    return {(worldPos.x - pos.x) * zoom, screen_size.h - ((worldPos.y - pos.y) * zoom)};
}

void Camera::setTarget(PositionComponent* p){
    target = p;
}

void Camera::setTarget(Entity e){
    target = &e.getComponent<PositionComponent>();
}

const ShapeRectangle& Camera::getCameraShape(){
    shape.pos = pos;
    shape.size.w = (screen_size.w / zoom);
    shape.size.h = (screen_size.h / zoom);
    return shape;
}

void Camera::updateCameraMatrix(){
    cameraMatrix = glm::mat4(1.0f);
    cameraMatrix = glm::scale(cameraMatrix, glm::vec3(zoom, zoom, 1.0f));
    cameraMatrix = glm::translate(cameraMatrix, glm::vec3(-pos.x, -pos.y, 0.0f));
}
