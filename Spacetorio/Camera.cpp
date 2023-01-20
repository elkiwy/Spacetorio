#include "Camera.hpp"
#include "Components_position.hpp"
#include "Texture.hpp"
#include "Utils_points.hpp"
#include "Renderer.hpp"
#include <iostream>

Camera::Camera() {
    std::cout << "Initializing camera" << std::endl;
    if (global_renderer != nullptr){
        iSize sz = global_renderer->getScreenSize();
        screen_size = fSize(sz.w, sz.h);
        moveTo(0, 0);
    }
    std::cout << "done camera" << std::endl;
}
Camera::~Camera() {}

void Camera::update(const Uint8 *ks) {
    if (this->target == nullptr){
        //Free moving
        spd.dampVector(0.95f);
        pos += spd;
    }else{
        //Locked to target
        this->moveTo(target->pos.x, target->pos.y);
    }
}

void Camera::moveTo(float x, float y) {
    pos.x = x - (screen_size.w * 0.5f)/zoom;
    pos.y = y - (screen_size.h * 0.5f)/zoom;
}

void Camera::moveBy(float dx, float dy) {
    pos.x += dx;
    pos.y += dy;
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
    moveBy(-diff.x / zoom, -diff.y / zoom);
}

fPoint Camera::screenToWorld(fPoint screenPos) const {
    return {(screenPos.x / zoom) + pos.x, (screenPos.y / zoom) + pos.y};
}

fPoint Camera::worldToScreen(fPoint worldPos) const {
    return {(worldPos.x - pos.x) * zoom, (worldPos.y - pos.y) * zoom};
}

void Camera::setTarget(PositionComponent* p){
    target = p;
}

void Camera::setTarget(Entity e){
    target = &e.getComponent<PositionComponent>();
}
