#include "Planet.hpp"
#include <iostream>
#include "Utils_math.hpp"

Planet::Planet(){
    //Generate a random planet
    pos.x = randFloat(-100.0, 100.0);
    pos.y = randFloat(-100.0, 100.0);
    size = randFloat(20.0, 30.0);
}

Planet::~Planet(){

}


const Scene& Planet::getCurrentScene(){
    return *currentScene;
}

void Planet::render(const Camera& cam){
    global_renderer->drawCircle(pos.x - cam.pos.x, pos.y - cam.pos.y, size, {255, 0, 0, 255});
}
