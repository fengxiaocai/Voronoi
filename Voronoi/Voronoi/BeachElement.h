//
//  BeachElement.h
//  Voronoi
//
//  Created by Ayush Tiwari on 28/09/19.
//  Copyright © 2019 Ayush Tiwari. All rights reserved.
//

#pragma once

#include "VoronoiDiagram.h"

class EventPoint;

///Class for an Element in the BeacLine.
struct BeachElement
{
    enum class Color{RED, BLACK};

    // Hierarchy
    BeachElement* parent;
    BeachElement* left;
    BeachElement* right;
    
    // Diagram
    VoronoiDiagram::Site* site;
    VoronoiDiagram::HalfEdge* leftHalfEdge;
    VoronoiDiagram::HalfEdge* rightHalfEdge;
    EventPoint* event;
    
    // Optimizations
    BeachElement* prev;
    BeachElement* next;
    
    // Only for balancing
    Color color;
};
