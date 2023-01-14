#include "Utils_geometry.hpp"



inline float deg2rad(float d) {
    return d * (M_PI / 180.0f);
}
inline float rad2deg(float r) {
    return r * (180.0f / M_PI);
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
ShapeRectangle::~ShapeRectangle(){}

bool ShapeRectangle::checkCollisionWithLine(const ShapeLine& other) const{
    // TODO
}

bool ShapeRectangle::checkCollisionWithCircle(const ShapeCircle& other) const{
    return checkCircleRectangle(other, *this);
}

bool ShapeRectangle::checkCollisionWithRectangle(const ShapeRectangle& other) const{
    // TODO
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

bool ShapeLine::checkCollisionWithLine(const ShapeLine& other) const{
    // TODO
}

bool ShapeLine::checkCollisionWithCircle(const ShapeCircle& other) const{
    return checkSegmentCircle(*this, other);
}

bool ShapeLine::checkCollisionWithRectangle(const ShapeRectangle& other) const{
    // TODO
}
