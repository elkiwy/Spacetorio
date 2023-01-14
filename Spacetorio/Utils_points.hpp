#ifndef UTILS_POINTS_H_
#define UTILS_POINTS_H_

#include "SDL_stdinc.h"
#include <cstdlib>
#include <deque>
#include <ostream>
#include <iostream>

#ifdef __APPLE__
#else
#include <xstring>
#endif

inline float deg2rad(float d);
inline float rad2deg(float r);

/*
** fPoint
*/

class fPoint {
    public:
        fPoint(float x, float y) : x(x), y(y) {}
        virtual ~fPoint() = default;

        float distTo(const fPoint other) const;
        float distSquaredTo(const fPoint other) const;
        void dampVector(float factor, float zeroThreshold = 0.01f);
        fPoint rotatedPoint(float ang, fPoint orig) const;
        bool inRect(float rx, float ry, float rw, float rh) const;
        float dot(const fPoint& other) const;
        float length() const;

        fPoint& operator+=(const fPoint& other);
        fPoint& operator-=(const fPoint& other);
        fPoint operator*(float v);
        fPoint operator+(fPoint& other);
        fPoint operator-(fPoint& other);
        fPoint operator*(float v) const;
        fPoint operator+(const fPoint& other) const;
        fPoint operator-(const fPoint& other) const;

        float x;
        float y;
};

/*
** iPoint
*/

class iPoint {
    public:
        iPoint(int x, int y) : x(x), y(y) {}
        virtual ~iPoint() = default;

        int x;
        int y;
};

/*
** fSize
*/

class fSize {
    public:
        fSize(float w, float h) : w(w), h(h) {}
        virtual ~fSize() = default;

        fSize operator+(fSize& other);
        fSize operator-(fSize& other);
        fSize operator+(const fSize& other);
        fSize operator-(const fSize& other);

        float w;
        float h;
};

/*
** iSize
*/

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

#endif // UTILS_POINTS_H_
