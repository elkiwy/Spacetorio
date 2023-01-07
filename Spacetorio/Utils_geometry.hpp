#ifndef UTILS_GEOMETRY_H_
#define UTILS_GEOMETRY_H_

class fPoint {
    public:
        fPoint(float x, float y) : x(x), y(y) {}
        virtual ~fPoint() = default;

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


#endif // UTILS_GEOMETRY_H_
