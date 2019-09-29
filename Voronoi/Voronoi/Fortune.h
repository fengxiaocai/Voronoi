//
//  Fortune.h
//  Voronoi
//
//  Created by Ayush Tiwari on 28/09/19.
//  Copyright © 2019 Ayush Tiwari. All rights reserved.
//

#pragma once

#include "Heap.h"
#include "VoronoiDiagram.h"
#include "BeachTree.h"

class BeachElement;
class EventPoint;

/// Class contains functions to execute the algorithm.
class Fortune
{
public:

    Fortune(std::vector<EuclidVec> points);
    ~Fortune();

    void build();
    bool bound(Boundary box);

    VoronoiDiagram getDiagram();

private:
    VoronoiDiagram mDiagram;
    BeachTree mBeachline;
    Heap<EventPoint> mEvents;
    double mBeachlineY;

    // Algorithm
    void handleSiteEvent(EventPoint* event);
    void handleCircleEvent(EventPoint* event);

    // Arcs
    BeachElement* breakArc(BeachElement* arc, VoronoiDiagram::Site* site);
    void removeArc(BeachElement* arc, VoronoiDiagram::Vertex* vertex);

    // Breakpoint
    bool isMovingRight(const BeachElement* left, const BeachElement* right) const;
    double getInitialX(const BeachElement* left, const BeachElement* right, bool movingRight) const;

    // Edges
    void addEdge(BeachElement* left, BeachElement* right);
    void setOrigin(BeachElement* left, BeachElement* right, VoronoiDiagram::Vertex* vertex);
    void setDestination(BeachElement* left, BeachElement* right, VoronoiDiagram::Vertex* vertex);
    void setPrevHalfEdge(VoronoiDiagram::HalfEdge* prev, VoronoiDiagram::HalfEdge* next);

    // Events
    void addEvent(BeachElement* left, BeachElement* middle, BeachElement* right);
    void deleteEvent(BeachElement* arc);
    EuclidVec computeConvergencePoint(const EuclidVec& point1, const EuclidVec& point2, const EuclidVec& point3, double& y) const;

    // Bounding

    struct LinkedVertex
    {
        VoronoiDiagram::HalfEdge* prevHalfEdge;
        VoronoiDiagram::Vertex* vertex;
        VoronoiDiagram::HalfEdge* nextHalfEdge;
    };
};
