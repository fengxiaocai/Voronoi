//
//  EuclidVec.h
//  Voronoi
//
//  Created by Ayush Tiwari on 28/09/19.
//  Copyright © 2019 Ayush Tiwari. All rights reserved.
//

#pragma once

#include <ostream>

// Declarations
class EuclidVec;
EuclidVec operator-(EuclidVec lhs, const EuclidVec& rhs);

/// Class Represent an Euclidean Vector (Point) in 2D Space.
class EuclidVec
{
public:
    double x;
    double y;

    EuclidVec(double x = 0.0, double y = 0.0);

    // Unary operators

    EuclidVec operator-() const;
    EuclidVec& operator+=(const EuclidVec& other);
    EuclidVec& operator-=(const EuclidVec& other);
    EuclidVec& operator*=(double t);

    // Other operations

    EuclidVec getOrthogonal() const;
    double dot(const EuclidVec& other) const;
    double getNorm() const;
    double getDistance(const EuclidVec& other) const;
    double getDet(const EuclidVec& other) const;
};

// Binary operators

EuclidVec operator+(EuclidVec lhs, const EuclidVec& rhs);
EuclidVec operator-(EuclidVec lhs, const EuclidVec& rhs);
EuclidVec operator*(double t, EuclidVec vec);
EuclidVec operator*(EuclidVec vec, double t);
std::ostream& operator<<(std::ostream& os, const EuclidVec& vec);
