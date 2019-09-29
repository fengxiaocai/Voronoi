//
//  VoronoiDiagram.h
//  Voronoi
//
//  Created by Ayush Tiwari on 28/09/19.
//  Copyright © 2019 Ayush Tiwari. All rights reserved.
//

#pragma once

#include <vector>
#include <list>

#include "Boundary.h"

class Fortune;

/**
    DCEL Implementation.
 */
class VoronoiDiagram
{
public:
    struct HalfEdge;
    struct Face;

    struct Site
    {
        std::size_t index;
        EuclidVec point;
        Face* face;
    };

    struct Vertex
    {
        EuclidVec point;

    private:
        friend VoronoiDiagram;
        std::list<Vertex>::iterator it;
    };

    struct HalfEdge
    {
        Vertex* origin = nullptr;
        Vertex* destination = nullptr;
        HalfEdge* twin = nullptr;
        Face* incidentFace;
        HalfEdge* prev = nullptr;
        HalfEdge* next = nullptr;

    private:
        friend VoronoiDiagram;
        std::list<HalfEdge>::iterator it;
    };

    struct Face
    {
        Site* site;
        HalfEdge* innerHalfEdge;
    };

    VoronoiDiagram(const std::vector<EuclidVec>& points);

    // Remove copy operations
    VoronoiDiagram(const VoronoiDiagram&) = delete;
    VoronoiDiagram& operator=(const VoronoiDiagram&) = delete;

    // Move operations
    VoronoiDiagram(VoronoiDiagram&&) = default;
    VoronoiDiagram& operator=(VoronoiDiagram&&) = default;

    // Get Functions
    Site* getSite(std::size_t i);
    std::size_t getSitesCount() const;
    Face* getFace(std::size_t i);
    const std::list<Vertex>& getVertices() const;
    const std::list<HalfEdge>& getHalfEdges() const;

    // Taking care of intersections with the Boundary
    bool intersect(Boundary boundary);

private:
    std::vector<Site> Sites;
    std::vector<Face> Faces;
    std::list<Vertex> Vertices;
    std::list<HalfEdge> HalfEdges;

    // Diagram construction
    friend Fortune;

    Vertex* createVertex(EuclidVec point);
    Vertex* createCorner(Boundary box, Boundary::Side side);
    HalfEdge* createHalfEdge(Face* face);

    // Intersection with a box
    void link(Boundary box, HalfEdge* start, Boundary::Side startSide, HalfEdge* end, Boundary::Side endSide);
    void removeVertex(Vertex* vertex);
    void removeHalfEdge(HalfEdge* halfEdge);
};
