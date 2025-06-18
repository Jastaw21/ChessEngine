//
// Created by jacks on 16/06/2025.
//

#ifndef VEC2D_H
#define VEC2D_H


class Vec2D {
public:

    float x, y;

    Vec2D operator+(const Vec2D &v) const{ return Vec2D(x + v.x, y + v.y); }
    Vec2D operator-(const Vec2D &v) const{ return Vec2D(x - v.x, y - v.y); }
    Vec2D operator/(const float &v) const{ return Vec2D(x / v, y / v); }

    Vec2D operator+=(const Vec2D &rhs){
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    Vec2D operator-=(const Vec2D &rhs){
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }
};


#endif //VEC2D_H
