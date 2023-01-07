#include "Universe.hpp"

Universe::Universe(){
    //Create some planets
    for(int i=0; i<10; i++){
        planets.emplace_back();
    }
}

Universe::~Universe(){

}

const Scene& Universe::getCurrentScene(){
    return (currentPlanet == nullptr) ? spaceScene : currentPlanet->getCurrentScene();
}
