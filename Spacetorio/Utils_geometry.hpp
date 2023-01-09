#ifndef UTILS_GEOMETRY_H_
#define UTILS_GEOMETRY_H_

#include <cstdlib>
#include <deque>
#include <ostream>
#include <iostream>
#include <xstring>

class fPoint {
    public:
        fPoint(float x, float y) : x(x), y(y) {}
        virtual ~fPoint() = default;

        void dampVector(float factor, float zeroThreshold = 0.01f){
            x *= factor;
            y *= factor;

            //checks for zero correction
            if (abs(x) < zeroThreshold){x = 0.0f;}
            if (abs(y) < zeroThreshold){y = 0.0f;}
        }

        fPoint& operator+=(const fPoint& other){
            x += other.x;
            y += other.y;
            return *this;
        }

        fPoint& operator-=(const fPoint& other){
            x -= other.x;
            y -= other.y;
            return *this;
        }

        fPoint operator+(fPoint& other){return {x+other.x, y+other.y};}
        fPoint operator-(fPoint& other){return {x-other.x, y-other.y};}

        float x;
        float y;
};

class iPoint {
    public:
        iPoint(int x, int y) : x(x), y(y) {}
        virtual ~iPoint() = default;

        int x;
        int y;
};

class fSize {
    public:
        fSize(float w, float h) : w(w), h(h) {}
        virtual ~fSize() = default;

        fSize operator+(fSize& other){return {w+other.w, h+other.h};}
        fSize operator-(fSize& other){return {w-other.w, h-other.h};}
        fSize operator+(const fSize& other){return {w+other.w, h+other.h};}
        fSize operator-(const fSize& other){return {w-other.w, h-other.h};}

        float w;
        float h;
};

class iSize {
    public:
        iSize(int w, int h) : w(w), h(h) {}
        virtual ~iSize() = default;

        int w;
        int h;
};


std::ostream &operator<<(std::ostream &os, fPoint const &p);
std::ostream &operator<<(std::ostream &os, fSize const &p);

using fVec = fPoint;

#endif // UTILS_GEOMETRY_H_
