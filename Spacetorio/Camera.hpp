#ifndef CAMERA_H_
#define CAMERA_H_

#include "Utils_geometry.hpp"
#include <ostream>

class Camera {
    public:
        Camera(){
            moveTo(0, 0);
        }
        virtual ~Camera(){}

        void update() {
            spd.dampVector(0.95f);

            pos += spd;
        }

        void moveTo(float x, float y){
            pos.x = 0;//-screen_size.w*0.5f;
            pos.y = 0;//-screen_size.h*0.5f;
        }

        void moveBy(float dx, float dy){
            pos.x += dx;
            pos.y += dy;
        }

        void addSpd(float dx, float dy){
            spd.x += dx;
            spd.y += dy;
        }

        void zoomBy(float dy, fPoint screenRef){
            fPoint worldRef = screenToWorld(screenRef);
            zoom += dy;
            fPoint movedScreenRef = worldToScreen(worldRef);

            //screenRef should remain fixed, so we need the diff
            //fVec diff = movedScreenRef - screenRef;
            fVec diff = screenRef - movedScreenRef;
            moveBy(-diff.x/zoom, -diff.y/zoom);
        }

        fPoint screenToWorld(fPoint screenPos) const{
            return {
                (screenPos.x/zoom) + pos.x,
                (screenPos.y/zoom) + pos.y
            };
        }

        fPoint worldToScreen(fPoint worldPos) const{
            return {
                (worldPos.x-pos.x)*zoom,
                (worldPos.y-pos.y)*zoom
            };
        }



        fVec spd   = fVec(0.0f, 0.0f);
        fPoint pos = fPoint(0,0);
        fSize size = fSize(100,100);
        float zoom = 1.0f;
        fSize screen_size = {1280.0f, 720.0f};
        fSize viewport_size = {1280.0f, 720.0f};
};


#endif // CAMERA_H_
