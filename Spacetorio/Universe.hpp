#ifndef UNIVERSE_H_
#define UNIVERSE_H_

#include <vector>
#include "Planet.hpp"

class Universe {
    public:
        Universe();
        virtual ~Universe();

        Scene& getCurrentScene();

    private:
        std::vector<Planet> planets = {};
        Planet* currentPlanet = nullptr;
        Scene spaceScene;
};



#endif // UNIVERSE_H_
