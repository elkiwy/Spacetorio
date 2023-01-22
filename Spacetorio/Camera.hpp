#ifndef CAMERA_H_
#define CAMERA_H_

#include "SDL_stdinc.h"
#include "Utils_points.hpp"
#include "Utils_geometry.hpp"
#include "Entity.hpp"
#include <ostream>


class Renderer;
extern Renderer* global_renderer;

class PositionComponent;

class Camera {
    public:
        Camera();
        virtual ~Camera();

        void init();
        void update(const Uint8* ks);

        fPoint getCameraWorldCenter() const;
        void moveTo(float x, float y);
        void moveBy(float dx, float dy);
        void addSpd(float dx, float dy);

        void zoomBy(float dy, fPoint screenRef);

        fPoint screenToWorld(fPoint screenPos) const;
        fPoint worldToScreen(fPoint worldPos) const;

        void setTarget(PositionComponent* p);
        void setTarget(Entity e);

        const ShapeRectangle& getCameraShape();



        fVec   spd = fVec(0.0f, 0.0f);
        fPoint pos = fPoint(0,0);
        float zoom = 1.0f;
        fSize screen_size = {0.0f, 0.0f};

        ShapeRectangle shape;
        PositionComponent* target = nullptr;

};


#endif // CAMERA_H_
