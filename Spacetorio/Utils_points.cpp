#include "Utils_points.hpp"

#ifdef __WIN32__
#include <corecrt_math.h>
#endif


std::ostream &operator<<(std::ostream &os, fPoint const &p) {
    return os << "{" << p.x << "," << p.y << "}";
}
std::ostream &operator<<(std::ostream &os, fSize const &p) {
    return os << "{" << p.w << "," << p.h << "}";
}

inline float deg2rad(float d) {
    return d * (M_PI / 180.0f);
}
inline float rad2deg(float r) {
    return r * (180.0f / M_PI);
}

/*
** fPoint
*/


float fPoint::distTo(const fPoint other) const{
    return sqrt((this->x-other.x)*(this->x-other.x) + (this->y-other.y)*(this->y-other.y));
}

float fPoint::distSquaredTo(const fPoint other) const{
    return (this->x-other.x)*(this->x-other.x) + (this->y-other.y)*(this->y-other.y);
}

void fPoint::dampVector(float factor, float zeroThreshold){
    x *= factor; y *= factor;

    //checks for zero correction
    if (abs(x) < zeroThreshold){x = 0.0f;}
    if (abs(y) < zeroThreshold){y = 0.0f;}
}

fPoint fPoint::rotatedPoint(float ang, fPoint orig) const{
    float s = sin(ang); float c = cos(ang);
    fPoint pt = {this->x - orig.x, this->y - orig.y};
    fPoint newPt = {pt.x*c - pt.y*s, pt.x*s + pt.y*c};
    newPt += orig;
    return newPt;
}

bool fPoint::inRect(float rx, float ry, float rw, float rh) const{
    return (x >= rx && x <= rx + rw && y >= ry && y <= ry + rh );
}

float fPoint::dot(const fPoint& other) const{
    return x*other.x + y*other.y;
}
float fPoint::length() const{
    return sqrt(x*x + y*y);
}


fPoint fPoint::movedByTo(float dist, float angleRad) const {
    return fPoint(this->x + dist * cos(angleRad), this->y + dist * sin(angleRad));
}

fPoint &fPoint::operator+=(const fPoint &other) {x += other.x; y += other.y; return *this;}
fPoint &fPoint::operator-=(const fPoint &other) {x -= other.x; y -= other.y; return *this;}
fPoint fPoint::operator*(float v){return {x*v, y*v};}
fPoint fPoint::operator+(fPoint& other){return {x+other.x, y+other.y};}
fPoint fPoint::operator-(fPoint& other){return {x-other.x, y-other.y};}
fPoint fPoint::operator*(float v) const{return {x*v, y*v};}
fPoint fPoint::operator+(const fPoint& other) const{return {x+other.x, y+other.y};}
fPoint fPoint::operator-(const fPoint& other) const{return {x-other.x, y-other.y};}




/*
** fSize
*/

fSize fSize::operator+(fSize& other){return {w+other.w, h+other.h};}
fSize fSize::operator-(fSize& other){return {w-other.w, h-other.h};}
fSize fSize::operator+(const fSize& other){return {w+other.w, h+other.h};}
fSize fSize::operator-(const fSize& other){return {w-other.w, h-other.h};}
fSize fSize::operator*(float v) const{return {w*v, h*v};}
