//
//  Boundary.h
//  Voronoi
//
//  Created by Ayush Tiwari on 28/09/19.
//  Copyright © 2019 Ayush Tiwari. All rights reserved.
//

#pragma once

#include <array>
#include <limits>

#include "EuclidVec.h"

/// Contains functions that help bound the Voronoi Diagram.
class Boundary
{
public:
    
    // Be careful, y-axis is oriented to the top like in math
    enum class Side : int {LEFT, BOTTOM, RIGHT, TOP};
    
    // Representation of Intersection Points
    struct Intersection
    {
        Side side;
        EuclidVec point;
    };

    double left;
    double bottom;
    double right;
    double top;
    
    bool contains(const EuclidVec& point) const;
    Intersection getFirstIntersection(const EuclidVec& origin, const EuclidVec& direction) const; // Useful for Fortune's algorithm
    int getIntersections(const EuclidVec& origin, const EuclidVec& destination, std::array<Intersection, 2>& intersections) const; // Useful for diagram intersection

private:
    static constexpr double EPSILON = std::numeric_limits<double>::epsilon();
};
