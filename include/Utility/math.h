//
// Created by jacks on 21/06/2025.
//

#ifndef MATH_H
#define MATH_H

namespace MathUtility {
    template<typename T>
    int sign(T a){ return a > 0 ? 1 : a < 0 ? -1 : 0; }

    template<typename T>
    float lerp(T value, T min, T max){
        if (value < min)
            return -1;
        if (value > max)
            return 1;
        return (value - min) / static_cast<float>(max - min);
    }

    inline float map(const float value, const float min, const float max, const float newMin, const float newMax){
        if (value < min)
            return newMin;
        if (value > max)
            return newMax;
        float inRange = (value - min) / (max - min);
        return newMin + inRange * (newMax - newMin);
    }
}


#endif //MATH_H
