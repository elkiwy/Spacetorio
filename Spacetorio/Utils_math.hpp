#ifndef UTILS_MATH_H_
#define UTILS_MATH_H_

#include <stdlib.h>

inline int randInt(int min, int max){
    return rand() % (max-min+1) + min;
}

inline float randFloat(){
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

inline float randFloat(float min, float max){
    return min + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(max-min)));
}


#endif // UTILS_MATH_H_
