//
//  EventPoint.h
//  Voronoi
//
//  Created by Ayush Tiwari on 28/09/19.
//  Copyright © 2019 Ayush Tiwari. All rights reserved.
//

#pragma once

#include "EuclidVec.h"
#include "VoronoiDiagram.h"

class BeachElement;

/// Class Describing an Event Point
class EventPoint
{
public:
    enum class Type{SITE, CIRCLE};

    // Site event
    EventPoint(VoronoiDiagram::Site* site);
    // Circle event
    EventPoint(double y, EuclidVec point, BeachElement* arc);

    const Type type;
    double y;
    int index;
    
    // Site event
    VoronoiDiagram::Site* site;
    
    // Circle event
    EuclidVec point;
    BeachElement* arc;

};

bool operator<(const EventPoint& lhs, const EventPoint& rhs);
std::ostream& operator<<(std::ostream& os, const EventPoint& event);
