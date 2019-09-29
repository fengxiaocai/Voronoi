//
//  VoronoiDiagram.cpp
//  Voronoi
//
//  Created by Ayush Tiwari on 28/09/19.
//  Copyright © 2019 Ayush Tiwari. All rights reserved.
//

#include "VoronoiDiagram.h"

#include <unordered_set>

VoronoiDiagram::VoronoiDiagram(const std::vector<EuclidVec>& points)
{
    Sites.reserve(points.size());
    Faces.reserve(points.size());
    for(std::size_t i = 0; i < points.size(); ++i)
    {
        Sites.push_back(VoronoiDiagram::Site{i, points[i], nullptr});
        Faces.push_back(VoronoiDiagram::Face{&Sites.back(), nullptr});
        Sites.back().face = &Faces.back();
    }
}

VoronoiDiagram::Site* VoronoiDiagram::getSite(std::size_t i)
{
    return &Sites[i];
}

std::size_t VoronoiDiagram::getSitesCount() const
{
    return Sites.size();
}

VoronoiDiagram::Face* VoronoiDiagram::getFace(std::size_t i)
{
    return &Faces[i];
}

const std::list<VoronoiDiagram::Vertex>& VoronoiDiagram::getVertices() const
{
    return Vertices;
}

const std::list<VoronoiDiagram::HalfEdge>& VoronoiDiagram::getHalfEdges() const
{
    return HalfEdges;
}

bool VoronoiDiagram::intersect(Boundary box)
{
    bool error = false;
    std::unordered_set<HalfEdge*> processedHalfEdges;
    std::unordered_set<Vertex*> verticesToRemove;
    for (const Site& site : Sites)
    {
        HalfEdge* halfEdge = site.face->innerHalfEdge;
        bool inside = box.contains(halfEdge->origin->point);
        bool outerComponentDirty = !inside;
        HalfEdge* incomingHalfEdge = nullptr; // First half edge coming in the box
        HalfEdge* outgoingHalfEdge = nullptr; // Last half edge going out the box
        Boundary::Side incomingSide, outgoingSide;
        do
        {
            std::array<Boundary::Intersection, 2> intersections;
            int nbIntersections = box.getIntersections(halfEdge->origin->point, halfEdge->destination->point, intersections);
            bool nextInside = box.contains(halfEdge->destination->point);
            HalfEdge* nextHalfEdge = halfEdge->next;
            // The two points are outside the box
            if (!inside && !nextInside)
            {
                // The edge is outside the box
                if (nbIntersections == 0)
                {
                    verticesToRemove.emplace(halfEdge->origin);
                    removeHalfEdge(halfEdge);
                }
                // The edge crosses twice the frontiers of the box
                else if (nbIntersections == 2)
                {
                    verticesToRemove.emplace(halfEdge->origin);
                    if (processedHalfEdges.find(halfEdge->twin) != processedHalfEdges.end())
                    {
                        halfEdge->origin = halfEdge->twin->destination;
                        halfEdge->destination = halfEdge->twin->origin;
                    }
                    else
                    {
                        halfEdge->origin = createVertex(intersections[0].point);
                        halfEdge->destination = createVertex(intersections[1].point);
                    }
                    if (outgoingHalfEdge != nullptr)
                        link(box, outgoingHalfEdge, outgoingSide, halfEdge, intersections[0].side);
                    if (incomingHalfEdge == nullptr)
                    {
                       incomingHalfEdge = halfEdge;
                       incomingSide = intersections[0].side;
                    }
                    outgoingHalfEdge = halfEdge;
                    outgoingSide = intersections[1].side;
                    processedHalfEdges.emplace(halfEdge);
                }
                else
                    error = true;
            }
            // The edge is going outside the box
            else if (inside && !nextInside)
            {
                if (nbIntersections == 1)
                {
                    if (processedHalfEdges.find(halfEdge->twin) != processedHalfEdges.end())
                        halfEdge->destination = halfEdge->twin->origin;
                    else
                        halfEdge->destination = createVertex(intersections[0].point);
                    outgoingHalfEdge = halfEdge;
                    outgoingSide = intersections[0].side;
                    processedHalfEdges.emplace(halfEdge);
                }
                else
                    error = true;
            }
            // The edge is coming inside the box
            else if (!inside && nextInside)
            {
                if (nbIntersections == 1)
                {
                    verticesToRemove.emplace(halfEdge->origin);
                    if (processedHalfEdges.find(halfEdge->twin) != processedHalfEdges.end())
                        halfEdge->origin = halfEdge->twin->destination;
                    else
                        halfEdge->origin = createVertex(intersections[0].point);
                    if (outgoingHalfEdge != nullptr)
                        link(box, outgoingHalfEdge, outgoingSide, halfEdge, intersections[0].side);
                    if (incomingHalfEdge == nullptr)
                    {
                       incomingHalfEdge = halfEdge;
                       incomingSide = intersections[0].side;
                    }
                    processedHalfEdges.emplace(halfEdge);
                }
                else
                    error = true;
            }
            halfEdge = nextHalfEdge;
            // Update inside
            inside = nextInside;
        } while (halfEdge != site.face->innerHalfEdge);
        // Link the last and the first half edges inside the box
        if (outerComponentDirty && incomingHalfEdge != nullptr)
            link(box, outgoingHalfEdge, outgoingSide, incomingHalfEdge, incomingSide);
        // Set outer component
        if (outerComponentDirty)
            site.face->innerHalfEdge = incomingHalfEdge;
    }
    // Remove vertices
    for (auto& vertex : verticesToRemove)
        removeVertex(vertex);
    // Return the status
    return !error;
}

VoronoiDiagram::Vertex* VoronoiDiagram::createVertex(EuclidVec point)
{
    Vertices.emplace_back();
    Vertices.back().point = point;
    Vertices.back().it = std::prev(Vertices.end());
    return &Vertices.back();
}

VoronoiDiagram::Vertex* VoronoiDiagram::createCorner(Boundary box, Boundary::Side side)
{
    switch (side)
    {
        case Boundary::Side::LEFT:
            return createVertex(EuclidVec(box.left, box.top));
        case Boundary::Side::BOTTOM:
            return createVertex(EuclidVec(box.left, box.bottom));
        case Boundary::Side::RIGHT:
            return createVertex(EuclidVec(box.right, box.bottom));
        case Boundary::Side::TOP:
            return createVertex(EuclidVec(box.right, box.top));
        default:
            return nullptr;
    }
}

VoronoiDiagram::HalfEdge* VoronoiDiagram::createHalfEdge(Face* face)
{
    HalfEdges.emplace_back();
    HalfEdges.back().incidentFace = face;
    HalfEdges.back().it = std::prev(HalfEdges.end());
    if(face->innerHalfEdge == nullptr)
        face->innerHalfEdge = &HalfEdges.back();
    return &HalfEdges.back();
}

void VoronoiDiagram::link(Boundary box, HalfEdge* start, Boundary::Side startSide, HalfEdge* end, Boundary::Side endSide)
{
    HalfEdge* halfEdge = start;
    int side = static_cast<int>(startSide);
    while (side != static_cast<int>(endSide))
    {
        side = (side + 1) % 4;
        halfEdge->next = createHalfEdge(start->incidentFace);
        halfEdge->next->prev = halfEdge;
        halfEdge->next->origin = halfEdge->destination;
        halfEdge->next->destination = createCorner(box, static_cast<Boundary::Side>(side));
        halfEdge = halfEdge->next;
    }
    halfEdge->next = createHalfEdge(start->incidentFace);
    halfEdge->next->prev = halfEdge;
    end->prev = halfEdge->next;
    halfEdge->next->next = end;
    halfEdge->next->origin = halfEdge->destination;
    halfEdge->next->destination = end->origin;
}

void VoronoiDiagram::removeVertex(Vertex* vertex)
{
    Vertices.erase(vertex->it);
}

void VoronoiDiagram::removeHalfEdge(HalfEdge* halfEdge)
{
    HalfEdges.erase(halfEdge->it);
}
