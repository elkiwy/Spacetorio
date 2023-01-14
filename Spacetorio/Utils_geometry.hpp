#ifndef UTILS_GEOMETRY_H_
#define UTILS_GEOMETRY_H_

#include "SDL_stdinc.h"
#include "Utils_points.hpp"
#include <cstdlib>
#include <deque>
#include <ostream>
#include <iostream>

inline float deg2rad(float d);
inline float rad2deg(float r);


class ShapeLine;
class ShapeCircle;
class ShapeRectangle;

/*
** Shape
*/

class Shape{
    public:
        virtual bool checkCollision(const Shape& other) const = 0;
        virtual bool checkCollisionWithLine(const ShapeLine& other) const = 0;
        virtual bool checkCollisionWithCircle(const ShapeCircle& other) const = 0;
        virtual bool checkCollisionWithRectangle(const ShapeRectangle& other) const = 0;
};


/*
** ShapeLine
*/

class ShapeLine : public Shape{
    public:
        ShapeLine(float x1, float y1, float x2, float y2);
        ShapeLine(fPoint p1, fPoint p2);
        ~ShapeLine();

        bool checkCollision(const Shape& other) const;
        bool checkCollisionWithLine(const ShapeLine& other) const;
        bool checkCollisionWithCircle(const ShapeCircle& other) const;
        bool checkCollisionWithRectangle(const ShapeRectangle& other) const;

        fPoint p1;
        fPoint p2;
};
std::ostream &operator<<(std::ostream &os, ShapeLine const &p);


/*
** ShapeCircle
*/

class ShapeCircle : public Shape{
    public:
        ShapeCircle(float x, float y, float r);
        ShapeCircle(fPoint c, float r);
        ~ShapeCircle();

        bool checkCollision(const Shape& other) const;
        bool checkCollisionWithLine(const ShapeLine& other) const;
        bool checkCollisionWithCircle(const ShapeCircle& other) const;
        bool checkCollisionWithRectangle(const ShapeRectangle& other) const;

        fPoint c;
        float r;
};
std::ostream &operator<<(std::ostream &os, ShapeCircle const &p);



/*
** ShapeRectangle
*/

class ShapeRectangle : public Shape{
    public:
        ShapeRectangle(float x, float y, float w, float h);
        ShapeRectangle(fPoint pos, fSize size);
        ~ShapeRectangle();

        bool checkCollision(const Shape& other) const;
        bool checkCollisionWithLine(const ShapeLine& other) const;
        bool checkCollisionWithCircle(const ShapeCircle& other) const;
        bool checkCollisionWithRectangle(const ShapeRectangle& other) const;

        fPoint pos;
        fSize size;
};
std::ostream &operator<<(std::ostream &os, ShapeRectangle const &p);



#endif // UTILS_GEOMETRY_H_



