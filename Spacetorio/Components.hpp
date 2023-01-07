#ifndef COMPONENTS_H_
#define COMPONENTS_H_

#include <string>
#include "Utils_geometry.hpp"

struct TagComponent{
    std::string tag;

    TagComponent() = default;
    TagComponent(const TagComponent&) = default;
    TagComponent(const std::string& tag) : tag(tag) {}
};


struct PositionComponent{
    fPoint pos = {0,0};

    PositionComponent() = default;
    PositionComponent(const PositionComponent&) = default;
    PositionComponent(const fPoint& p) : pos(p) {}
    PositionComponent(float x, float y) : pos({x,y}) {}
};




#endif // COMPONENTS_H_
