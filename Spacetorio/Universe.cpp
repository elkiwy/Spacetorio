#include "Universe.hpp"

Universe::Universe(){
    //Create some planets
    for(int i=0; i<10; i++){
        //planets.emplace_back();
    }
}

Universe::~Universe(){

}

Scene& Universe::getCurrentScene(){
    return spaceScene;
    //return (currentPlanet == nullptr) ? spaceScene : currentPlanet->getCurrentScene();
}
