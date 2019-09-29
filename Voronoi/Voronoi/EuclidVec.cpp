//
//  EuclidVec.cpp
//  Voronoi
//
//  Created by Ayush Tiwari on 28/09/19.
//  Copyright © 2019 Ayush Tiwari. All rights reserved.
//
#include "EuclidVec.h"

#include <cmath>

EuclidVec::EuclidVec(double x, double y) : x(x), y(y)
{

}

// Unary operators

EuclidVec EuclidVec::operator-() const
{
    return EuclidVec(-x, -y);
}

EuclidVec& EuclidVec::operator+=(const EuclidVec& other)
{
    x += other.x;
    y += other.y;
    return *this;
}

EuclidVec& EuclidVec::operator-=(const EuclidVec& other)
{
    x -= other.x;
    y -= other.y;
    return *this;
}

EuclidVec& EuclidVec::operator*=(double t)
{
    x *= t;
    y *= t;
    return *this;
}

// Other operations

EuclidVec EuclidVec::getOrthogonal() const
{
    return EuclidVec(-y, x);
}

double EuclidVec::dot(const EuclidVec& other) const
{
    return x * other.x + y * other.y;
}

double EuclidVec::getNorm() const
{
    return std::sqrt(x * x + y * y);
}

double EuclidVec::getDistance(const EuclidVec& other) const
{
    return (*this - other).getNorm();
}

double EuclidVec::getDet(const EuclidVec& other) const
{
    return x * other.y - y * other.x;
}

EuclidVec operator+(EuclidVec lhs, const EuclidVec& rhs)
{
    lhs += rhs;
    return lhs;
}

EuclidVec operator-(EuclidVec lhs, const EuclidVec& rhs)
{
    lhs -= rhs;
    return lhs;
}

EuclidVec operator*(double t, EuclidVec vec)
{
    vec *= t;
    return vec;
}

EuclidVec operator*(EuclidVec vec, double t)
{
    return t * vec;
}

std::ostream& operator<<(std::ostream& os, const EuclidVec& vec)
{
    os << "(" << vec.x << ", " << vec.y << ")";
    return os;
}
