#include "Utils_geometry.hpp"
#include <algorithm>
#include <corecrt_math.h>


bool checkPointCircle(const ShapePoint& pt, const ShapeCircle& circ){
    return pt.p.distSquaredTo(circ.c) < circ.r*circ.r;
}

bool checkPointRect(const ShapePoint& pt, const ShapeRectangle& rect){
    return pt.p.inRect(rect.pos.x, rect.pos.y, rect.size.w, rect.size.h);
}


//Explanation: https://stackoverflow.com/questions/1073336/circle-line-segment-collision-detection-algorithm
bool checkSegmentCircle(const ShapeLine &line, const ShapeCircle &circ) {
    fVec d = line.p2 - line.p1;
    fVec f = line.p1 - circ.c;
    float a = d.dot(d);
    float b = 2 * f.dot(d);
    float c = f.dot(f) - circ.r * circ.r;

    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0) {
        return false;
    } else {
        discriminant = sqrt(discriminant);
        float t1 = (-b - discriminant) / (2 * a);
        float t2 = (-b + discriminant) / (2 * a);
        if ((t1 >= 0 && t1 <= 1) || (t2> 1 && t1 < 0) || (t2 >= 0 && t2 <= 1)) {
            return true;
        }
        return false;
    }
}

bool checkCircleRectangle(const ShapeCircle &circ, const ShapeRectangle &rect) {
    // INTERSECTING AN EDGE
    // Calculate distance between the edges, check intersection with nearest one
    float distTop    = circ.c.y - rect.pos.y;
    float distBottom = rect.pos.y + rect.size.h- circ.c.y;
    float distLeft   = circ.c.x - rect.pos.x;
    float distRight  = rect.pos.x + rect.size.w- circ.c.x;
    float distMin    = std::min({distTop, distBottom, distLeft, distRight});
    if (distMin == distTop && checkSegmentCircle(ShapeLine(rect.pos, rect.pos+fVec(rect.size.w,0.0f)), circ)){
        return true;
    } else if (distMin == distBottom && checkSegmentCircle(ShapeLine(rect.pos+fVec(0.0f,rect.size.h), rect.pos+fVec(rect.size.w,rect.size.h)), circ)){
        return true;
    } else if (distMin == distLeft && checkSegmentCircle(ShapeLine(rect.pos, rect.pos+fVec(0.0f,rect.size.h)), circ)){
        return true;
    } else if (distMin == distRight && checkSegmentCircle(ShapeLine(rect.pos+fVec(rect.size.w,0.0f), rect.pos+fVec(rect.size.w,rect.size.h)), circ)){
        return true;
    }

    // INTERSECTING A VERTEX
    // Closest point on rectangle to the center of the circle, and calculate distance to closest
    float closestX = std::max(rect.pos.x, std::min(circ.c.x, rect.pos.x + rect.size.w));
    float closestY = std::max(rect.pos.y, std::min(circ.c.y, rect.pos.y + rect.size.h));
    float distanceX = circ.c.x - closestX;
    float distanceY = circ.c.y - closestY;
    float distanceSquared = distanceX * distanceX + distanceY * distanceY;
    if (distanceSquared < circ.r * circ.r) {return true;}

    // INSIDE THE RECT
    if (circ.c.inRect(rect.pos.x, rect.pos.y, rect.size.w, rect.size.h)) {return true;}

    // None of the above
    return false;
}

bool checkLineLine(const ShapeLine& line1, const ShapeLine& line2){
    float d = (line1.p1.x - line1.p2.x) * (line2.p1.y - line2.p2.y) - (line1.p1.y - line1.p2.y) * (line1.p2.x - line2.p2.x);
    if (d == 0.0f || fabs(d) < 0.00001f){ return false;}
    float s = (1/d) *  ( (line1.p1.x - line1.p2.x) * line2.p1.y - (line1.p1.y - line1.p2.y) * line2.p1.x);
    float t = (1/d) * -(-(line1.p1.x - line1.p2.x) * line2.p2.y + (line1.p1.y - line1.p2.y) * line2.p2.x);
    return (s > 0 && s < 1 && t > 0 && t < 1);
}

bool checkRectangleRectangle(const ShapeRectangle &r1, const ShapeRectangle &r2) {
    return !(r2.pos.x > (r1.pos.x+r1.size.w) || (r2.pos.x+r2.size.w) < r1.pos.x || r2.pos.y > (r1.pos.y+r1.size.h) || (r2.pos.y+r2.size.h) < r1.pos.y);
}

/*
** ShapeCircle
*/

std::ostream &operator<<(std::ostream &os, ShapeCircle const &s) {
    return os << "{Circle " << s.c.x << "," << s.c.y <<  "," << s.r << "}";
}

ShapeCircle::ShapeCircle(float x, float y, float r) : c(x,y), r(r) {}
ShapeCircle::ShapeCircle(fPoint c, float r) : c(c), r(r) {}
ShapeCircle::~ShapeCircle(){}

bool ShapeCircle::checkCollision(const Shape& other) const{
    return other.checkCollisionWithCircle(*this);
}

bool ShapeCircle::checkCollisionWithPoint(const ShapePoint& other) const{
    return checkPointCircle(other, *this);
}

bool ShapeCircle::checkCollisionWithLine(const ShapeLine& other) const{
    return checkSegmentCircle(other, *this);
}

bool ShapeCircle::checkCollisionWithCircle(const ShapeCircle& other) const{
    return c.distSquaredTo(other.c) < (r + other.r)*(r + other.r);
}

bool ShapeCircle::checkCollisionWithRectangle(const ShapeRectangle& other) const{
    return checkCircleRectangle(*this, other);
}

/*
** ShapeRectangle
*/

std::ostream &operator<<(std::ostream &os, ShapeRectangle const &s) {
    return os << "{Rectangle " << s.pos.x << "," << s.pos.y <<  "," << s.size.w <<  "," << s.size.h << "}";
}

ShapeRectangle::ShapeRectangle(float x, float y, float w, float h) : pos(x,y), size(w,h){}
ShapeRectangle::ShapeRectangle(fPoint pos, fSize size) : pos(pos), size(size){}
ShapeRectangle::~ShapeRectangle(){}

bool ShapeRectangle::checkCollision(const Shape &other) const{
    return other.checkCollisionWithRectangle(*this);
}

bool ShapeRectangle::checkCollisionWithPoint(const ShapePoint& other) const{
    return checkPointRect(other, *this);
}

bool ShapeRectangle::checkCollisionWithLine(const ShapeLine& other) const{
    fPoint p1 = pos;
    fPoint p2 = pos+fPoint(size.w, 0.0f);
    fPoint p3 = pos+fPoint(size.w, size.h);
    fPoint p4 = pos+fPoint(0.0f, size.h);
    //Check if intersect any of the side
    if (checkLineLine(other, ShapeLine(p1, p2))){return true;}
    if (checkLineLine(other, ShapeLine(p2, p3))){return true;}
    if (checkLineLine(other, ShapeLine(p3, p4))){return true;}
    if (checkLineLine(other, ShapeLine(p4, p1))){return true;}
    //Check if totally inside
    return other.p1.inRect(pos.x, pos.y, size.w, size.h);
}

bool ShapeRectangle::checkCollisionWithCircle(const ShapeCircle& other) const{
    return checkCircleRectangle(other, *this);
}

bool ShapeRectangle::checkCollisionWithRectangle(const ShapeRectangle& other) const{
    return checkRectangleRectangle(*this, other);
}


/*
** ShapeLine
*/

std::ostream &operator<<(std::ostream &os, ShapeLine const &s) {
    return os << "{Line " << s.p1.x << "," << s.p1.y <<  "," << s.p2.x <<  "," << s.p2.y << "}";
}

ShapeLine::ShapeLine(float x1, float y1, float x2, float y2) : p1(x1,y1), p2(x2,y2){}
ShapeLine::ShapeLine(fPoint p1, fPoint p2) : p1(p1), p2(p2){}
ShapeLine::~ShapeLine(){}

bool ShapeLine::checkCollision(const Shape &other) const{
    return other.checkCollisionWithLine(*this);
}

bool ShapeLine::checkCollisionWithPoint(const ShapePoint& other) const{
    return false;
}

bool ShapeLine::checkCollisionWithLine(const ShapeLine& other) const{
    return checkLineLine(*this, other);
}

bool ShapeLine::checkCollisionWithCircle(const ShapeCircle& other) const{
    return checkSegmentCircle(*this, other);
}

bool ShapeLine::checkCollisionWithRectangle(const ShapeRectangle& other) const{
    fPoint p1 = other.pos;
    fPoint p2 = other.pos+fPoint(other.size.w, 0.0f);
    fPoint p3 = other.pos+fPoint(other.size.w, other.size.h);
    fPoint p4 = other.pos+fPoint(0.0f, other.size.h);
    //Check if intersect any of the side
    if (checkLineLine(*this, ShapeLine(p1, p2))){return true;}
    if (checkLineLine(*this, ShapeLine(p2, p3))){return true;}
    if (checkLineLine(*this, ShapeLine(p3, p4))){return true;}
    if (checkLineLine(*this, ShapeLine(p4, p1))){return true;}
    //Check if totally inside
    return this->p1.inRect(other.pos.x, other.pos.y, other.size.w, other.size.h);
}




/*
** ShapePoint
*/

std::ostream &operator<<(std::ostream &os, ShapePoint const &s) {
    return os << "{Point " << s.p.x << "," << s.p.y << "}";
}

ShapePoint::ShapePoint(float x, float y) : p(x,y){}
ShapePoint::ShapePoint(fPoint p) : p(p){}
ShapePoint::~ShapePoint(){}

bool ShapePoint::checkCollision(const Shape &other) const{
    return other.checkCollisionWithPoint(*this);
}

bool ShapePoint::checkCollisionWithPoint(const ShapePoint& other) const{
    return false;
}

bool ShapePoint::checkCollisionWithLine(const ShapeLine& other) const{
    return false;
}

bool ShapePoint::checkCollisionWithCircle(const ShapeCircle& other) const{
    return checkPointCircle(*this, other);
}

bool ShapePoint::checkCollisionWithRectangle(const ShapeRectangle& other) const{
    return checkPointRect(*this, other);
}
