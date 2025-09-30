//
// Created by jacks on 21/06/2025.
//

#ifndef MATH_H
#define MATH_H
#include <chrono>
#include <iomanip>

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

    inline uint64_t getXBits(uint64_t& number, int bits){ return (1 << bits) - 1 & number; }
}


namespace OtherUtility {
    inline std::string nowAsString(){
        using namespace std::chrono;

        const auto now = system_clock::now();
        auto itt = system_clock::to_time_t(now);
        std::tm bt = *std::localtime(&itt);

        // milliseconds part
        auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

        std::ostringstream oss;
        oss << std::put_time(&bt, "%H:%M:%S") // e.g. 14:37:12
                << '.' << std::setw(3) << std::setfill('0') << ms.count(); // +.015
        return oss.str();
    }
}

#endif //MATH_H
