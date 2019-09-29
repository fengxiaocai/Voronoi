//
//  EventPoint.cpp
//  Voronoi
//
//  Created by Ayush Tiwari on 28/09/19.
//  Copyright © 2019 Ayush Tiwari. All rights reserved.
//

#include "EventPoint.h"

EventPoint::EventPoint(VoronoiDiagram::Site* site) : type(Type::SITE), y(site->point.y), index(-1), site(site)
{

}

EventPoint::EventPoint(double y, EuclidVec point, BeachElement* arc) : type(Type::CIRCLE), y(y), index(-1), point(point), arc(arc)
{


}
bool operator<(const EventPoint& lhs, const EventPoint& rhs)
{
    return lhs.y < rhs.y;
}

std::ostream& operator<<(std::ostream& os, const EventPoint& event)
{
    if(event.type == EventPoint::Type::SITE)
        os << "S(" << event.site->index << ", " << event.y << ")";
    else
        os << "C(" << event.arc << ", " << event.y << ", " << event.point << ")";
    return os;
}
