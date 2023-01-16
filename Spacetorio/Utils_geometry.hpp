#ifndef UTILS_GEOMETRY_H_
#define UTILS_GEOMETRY_H_

#include "SDL_stdinc.h"
#include "Utils_points.hpp"
#include <cstdlib>
#include <deque>
#include <ostream>
#include <iostream>


inline float deg2rad(float d) {
    return d * (M_PI / 180.0f);
}
inline float rad2deg(float r) {
    return r * (180.0f / M_PI);
}



class ShapePoint;
class ShapeLine;
class ShapeCircle;
class ShapeRectangle;

/*
** Shape
*/

class Shape{
    public:
        virtual bool checkCollision(const Shape& other) const = 0;
        virtual bool checkCollisionWithPoint(const ShapePoint& other) const = 0;
        virtual bool checkCollisionWithLine(const ShapeLine& other) const = 0;
        virtual bool checkCollisionWithCircle(const ShapeCircle& other) const = 0;
        virtual bool checkCollisionWithRectangle(const ShapeRectangle& other) const = 0;
};


/*
** ShapePoint
*/

class ShapePoint : public Shape{
    public:
        ShapePoint() = default;
        ShapePoint(float x, float y);
        ShapePoint(fPoint p);
        ~ShapePoint();

        bool checkCollision(const Shape& other) const;
        bool checkCollisionWithPoint(const ShapePoint& other) const;
        bool checkCollisionWithLine(const ShapeLine& other) const;
        bool checkCollisionWithCircle(const ShapeCircle& other) const;
        bool checkCollisionWithRectangle(const ShapeRectangle& other) const;

        fPoint p = {0.0f, 0.0f};
};
std::ostream &operator<<(std::ostream &os, ShapeLine const &p);


/*
** ShapeLine
*/

class ShapeLine : public Shape{
    public:
        ShapeLine() = default;
        ShapeLine(float x1, float y1, float x2, float y2);
        ShapeLine(fPoint p1, fPoint p2);
        ~ShapeLine();

        bool checkCollision(const Shape& other) const;
        bool checkCollisionWithPoint(const ShapePoint& other) const;
        bool checkCollisionWithLine(const ShapeLine& other) const;
        bool checkCollisionWithCircle(const ShapeCircle& other) const;
        bool checkCollisionWithRectangle(const ShapeRectangle& other) const;

        fPoint p1 = {0.0f, 0.0f};
        fPoint p2 = {0.0f, 0.0f};
};
std::ostream &operator<<(std::ostream &os, ShapeLine const &p);


/*
** ShapeCircle
*/

class ShapeCircle : public Shape{
    public:
        ShapeCircle() = default;
        ShapeCircle(float x, float y, float r);
        ShapeCircle(fPoint c, float r);
        ~ShapeCircle();

        bool checkCollision(const Shape& other) const;
        bool checkCollisionWithPoint(const ShapePoint& other) const;
        bool checkCollisionWithLine(const ShapeLine& other) const;
        bool checkCollisionWithCircle(const ShapeCircle& other) const;
        bool checkCollisionWithRectangle(const ShapeRectangle& other) const;

        fPoint c = {0.0f, 0.0f};
        float r = 0.0f;
};
std::ostream &operator<<(std::ostream &os, ShapeCircle const &p);



/*
** ShapeRectangle
*/

class ShapeRectangle : public Shape{
    public:
        ShapeRectangle() = default;
        ShapeRectangle(float x, float y, float w, float h);
        ShapeRectangle(fPoint pos, fSize size);
        ~ShapeRectangle();

        bool checkCollision(const Shape& other) const;
        bool checkCollisionWithPoint(const ShapePoint& other) const;
        bool checkCollisionWithLine(const ShapeLine& other) const;
        bool checkCollisionWithCircle(const ShapeCircle& other) const;
        bool checkCollisionWithRectangle(const ShapeRectangle& other) const;

        fPoint pos = {0.0f, 0.0f};
        fSize size = {0.0f, 0.0f};
};
std::ostream &operator<<(std::ostream &os, ShapeRectangle const &p);



#endif // UTILS_GEOMETRY_H_



