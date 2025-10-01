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
    inline std::string getCurrentTimeString(){
        // Get current time
        auto now = std::chrono::system_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
        auto time = std::chrono::system_clock::to_time_t(now);

        // Convert to string
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }

    inline std::string getDateTimeString(std::chrono::steady_clock::time_point& timePoint){
        // Get the current time in system_clock as a baseline
        auto now_system = std::chrono::system_clock::now();
        auto now_steady = std::chrono::steady_clock::now();

        // Calculate the offset of the steady_clock time_point relative to now
        auto offset = timePoint - now_steady;
        auto system_time_point = now_system + offset;

        // Extract milliseconds and formatted time
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(system_time_point.time_since_epoch()) % 1000;
        auto time = std::chrono::system_clock::to_time_t(system_time_point);

        // Format the string
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }
}
#endif //MATH_H