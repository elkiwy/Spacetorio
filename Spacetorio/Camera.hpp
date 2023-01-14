#ifndef CAMERA_H_
#define CAMERA_H_

#include "SDL_stdinc.h"
#include "Utils_points.hpp"
#include "Entity.hpp"
#include <ostream>

class PositionComponent;

class Camera {
    public:
        Camera();
        virtual ~Camera();

        void update(const Uint8* ks) ;

        void moveTo(float x, float y);
        void moveBy(float dx, float dy);
        void addSpd(float dx, float dy);

        void zoomBy(float dy, fPoint screenRef);

        fPoint screenToWorld(fPoint screenPos) const;
        fPoint worldToScreen(fPoint worldPos) const;

        void setTarget(PositionComponent* p);
        void setTarget(Entity e);


        fVec   spd = fVec(0.0f, 0.0f);
        fPoint pos = fPoint(0,0);
        float zoom = 1.0f;
        fSize screen_size = {1280.0f, 720.0f};

        PositionComponent* target = nullptr;

};


#endif // CAMERA_H_
