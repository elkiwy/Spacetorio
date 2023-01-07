#ifndef CAMERA_H_
#define CAMERA_H_

#include "Utils_geometry.hpp"

class Camera {
    public:
        Camera();
        virtual ~Camera();

        fPoint pos = fPoint(0,0);
        fSize size = fSize(100,100);
        float zoom = 1.0f;
};


#endif // CAMERA_H_
