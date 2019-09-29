//
//  Fortune.cpp
//  Voronoi
//
//  Created by Ayush Tiwari on 28/09/19.
//  Copyright © 2019 Ayush Tiwari. All rights reserved.
//

#include "Fortune.h"
#include "BeachElement.h"
#include "EventPoint.h"

Fortune::Fortune(std::vector<EuclidVec> points) : mDiagram(std::move(points))
{

}

Fortune::~Fortune() = default;

void Fortune::build()
{
    // Initialize event queue
    for (std::size_t i = 0; i < mDiagram.getSitesCount(); ++i)
        mEvents.push(std::make_unique<EventPoint>(mDiagram.getSite(i)));

    // Process events
    while (!mEvents.isEmpty())
    {
        std::unique_ptr<EventPoint> event = mEvents.pop();
        mBeachlineY = event->y;
        if(event->type == EventPoint::Type::SITE)
            handleSiteEvent(event.get());
        else
            handleCircleEvent(event.get());
    }
}

VoronoiDiagram Fortune::getDiagram()
{
    return std::move(mDiagram);
}

void Fortune::handleSiteEvent(EventPoint* event)
{
    VoronoiDiagram::Site* site = event->site;
    // 1. Check if the bachline is empty
    if (mBeachline.isEmpty())
    {
        mBeachline.setRoot(mBeachline.createArc(site));
        return;
    }
    // 2. Look for the arc above the site
    BeachElement* arcToBreak = mBeachline.locateArcAbove(site->point, mBeachlineY);
    deleteEvent(arcToBreak);
    // 3. Replace this arc by the new arcs
    BeachElement* middleArc = breakArc(arcToBreak, site);
    BeachElement* leftArc = middleArc->prev;
    BeachElement* rightArc = middleArc->next;
    // 4. Add an edge in the diagram
    addEdge(leftArc, middleArc);
    middleArc->rightHalfEdge = middleArc->leftHalfEdge;
    rightArc->leftHalfEdge = leftArc->rightHalfEdge;
    // 5. Check circle events
    // Left triplet
    if (!mBeachline.isNil(leftArc->prev))
        addEvent(leftArc->prev, leftArc, middleArc);
    // Right triplet
    if (!mBeachline.isNil(rightArc->next))
        addEvent(middleArc, rightArc, rightArc->next);
}

void Fortune::handleCircleEvent(EventPoint* event)
{
    EuclidVec point = event->point;
    BeachElement* arc = event->arc;
    // 1. Add vertex
    VoronoiDiagram::Vertex* vertex = mDiagram.createVertex(point);
    // 2. Delete all the events with this arc
    BeachElement* leftArc = arc->prev;
    BeachElement* rightArc = arc->next;
    deleteEvent(leftArc);
    deleteEvent(rightArc);
    // 3. Update the beachline and the diagram
    removeArc(arc, vertex);
    // 4. Add new circle events
    // Left triplet
    if (!mBeachline.isNil(leftArc->prev))
        addEvent(leftArc->prev, leftArc, rightArc);
    // Right triplet
    if (!mBeachline.isNil(rightArc->next))
        addEvent(leftArc, rightArc, rightArc->next);
}

BeachElement* Fortune::breakArc(BeachElement* arc, VoronoiDiagram::Site* site)
{
    // Create the new subtree
    BeachElement* middleArc = mBeachline.createArc(site);
    BeachElement* leftArc = mBeachline.createArc(arc->site);
    leftArc->leftHalfEdge = arc->leftHalfEdge;
    BeachElement* rightArc = mBeachline.createArc(arc->site);
    rightArc->rightHalfEdge = arc->rightHalfEdge;
    // Insert the subtree in the beachline
    mBeachline.replace(arc, middleArc);
    mBeachline.insertBefore(middleArc, leftArc);
    mBeachline.insertAfter(middleArc, rightArc);
    // Delete old arc
    delete arc;
    // Return the middle arc
    return middleArc;
}

void Fortune::removeArc(BeachElement* arc, VoronoiDiagram::Vertex* vertex)
{
    // End edges
    setDestination(arc->prev, arc, vertex);
    setDestination(arc, arc->next, vertex);
    // Join the edges of the middle arc
    arc->leftHalfEdge->next = arc->rightHalfEdge;
    arc->rightHalfEdge->prev = arc->leftHalfEdge;
    // Update beachline
    mBeachline.remove(arc);
    // Create a new edge
    VoronoiDiagram::HalfEdge* prevHalfEdge = arc->prev->rightHalfEdge;
    VoronoiDiagram::HalfEdge* nextHalfEdge = arc->next->leftHalfEdge;
    addEdge(arc->prev, arc->next);
    setOrigin(arc->prev, arc->next, vertex);
    setPrevHalfEdge(arc->prev->rightHalfEdge, prevHalfEdge);
    setPrevHalfEdge(nextHalfEdge, arc->next->leftHalfEdge);
    // Delete node
    delete arc;
}

bool Fortune::isMovingRight(const BeachElement* left, const BeachElement* right) const
{
    return left->site->point.y < right->site->point.y;
}

double Fortune::getInitialX(const BeachElement* left, const BeachElement* right, bool movingRight) const
{
    return movingRight ? left->site->point.x : right->site->point.x;
}

void Fortune::addEdge(BeachElement* left, BeachElement* right)
{
    // Create two new half edges
    left->rightHalfEdge = mDiagram.createHalfEdge(left->site->face);
    right->leftHalfEdge = mDiagram.createHalfEdge(right->site->face);
    // Set the two half edges twins
    left->rightHalfEdge->twin = right->leftHalfEdge;
    right->leftHalfEdge->twin = left->rightHalfEdge;
}

void Fortune::setOrigin(BeachElement* left, BeachElement* right, VoronoiDiagram::Vertex* vertex)
{
    left->rightHalfEdge->destination = vertex;
    right->leftHalfEdge->origin = vertex;
}

void Fortune::setDestination(BeachElement* left, BeachElement* right, VoronoiDiagram::Vertex* vertex)
{
    left->rightHalfEdge->origin = vertex;
    right->leftHalfEdge->destination = vertex;
}

void Fortune::setPrevHalfEdge(VoronoiDiagram::HalfEdge* prev, VoronoiDiagram::HalfEdge* next)
{
    prev->next = next;
    next->prev = prev;
}

void Fortune::addEvent(BeachElement* left, BeachElement* middle, BeachElement* right)
{
    double y;
    EuclidVec convergencePoint = computeConvergencePoint(left->site->point, middle->site->point, right->site->point, y);
    bool isBelow = y <= mBeachlineY;
    bool leftBreakpointMovingRight = isMovingRight(left, middle);
    bool rightBreakpointMovingRight = isMovingRight(middle, right);
    double leftInitialX = getInitialX(left, middle, leftBreakpointMovingRight);
    double rightInitialX = getInitialX(middle, right, rightBreakpointMovingRight);
    bool isValid =
        ((leftBreakpointMovingRight && leftInitialX < convergencePoint.x) ||
        (!leftBreakpointMovingRight && leftInitialX > convergencePoint.x)) &&
        ((rightBreakpointMovingRight && rightInitialX < convergencePoint.x) ||
        (!rightBreakpointMovingRight && rightInitialX > convergencePoint.x));
    if (isValid && isBelow)
    {
        std::unique_ptr<EventPoint> event = std::make_unique<EventPoint>(y, convergencePoint, middle);
        middle->event = event.get();
        mEvents.push(std::move(event));
    }
}

void Fortune::deleteEvent(BeachElement* arc)
{
    if (arc->event != nullptr)
    {
        mEvents.remove(arc->event->index);
        arc->event = nullptr;
    }
}

EuclidVec Fortune::computeConvergencePoint(const EuclidVec& point1, const EuclidVec& point2, const EuclidVec& point3, double& y) const
{
    EuclidVec v1 = (point1 - point2).getOrthogonal();
    EuclidVec v2 = (point2 - point3).getOrthogonal();
    EuclidVec delta = 0.5 * (point3 - point1);
    double t = delta.getDet(v2) / v1.getDet(v2);
    EuclidVec center = 0.5 * (point1 + point2) + t * v1;
    double r = center.getDistance(point1);
    y = center.y - r;
    return center;
}

// Bound

#include <list>
#include <unordered_map>

bool Fortune::bound(Boundary box)
{
    // Make sure the bounding box contains all the vertices
    for (const auto& vertex : mDiagram.getVertices()) // Much faster when using vector<unique_ptr<Vertex*>, maybe we can test vertices in border cells to speed up
    {
        box.left = std::min(vertex.point.x, box.left);
        box.bottom = std::min(vertex.point.y, box.bottom);
        box.right = std::max(vertex.point.x, box.right);
        box.top = std::max(vertex.point.y, box.top);
    }
    // Retrieve all non bounded half edges from the beach line
    std::list<LinkedVertex> linkedVertices;
    std::unordered_map<std::size_t, std::array<LinkedVertex*, 8>> vertices(mDiagram.getSitesCount());
    if (!mBeachline.isEmpty())
    {
        BeachElement* leftArc = mBeachline.getLeftmostArc();
        BeachElement* rightArc = leftArc->next;
        while (!mBeachline.isNil(rightArc))
        {
            // Bound the edge
            EuclidVec direction = (leftArc->site->point - rightArc->site->point).getOrthogonal();
            EuclidVec origin = (leftArc->site->point + rightArc->site->point) * 0.5f;
            // Line-box intersection
            Boundary::Intersection intersection = box.getFirstIntersection(origin, direction);
            // Create a new vertex and ends the half edges
            VoronoiDiagram::Vertex* vertex = mDiagram.createVertex(intersection.point);
            setDestination(leftArc, rightArc, vertex);
            // Initialize pointers
            if (vertices.find(leftArc->site->index) == vertices.end())
                vertices[leftArc->site->index].fill(nullptr);
            if (vertices.find(rightArc->site->index) == vertices.end())
                vertices[rightArc->site->index].fill(nullptr);
            // Store the vertex on the boundaries
            linkedVertices.emplace_back(LinkedVertex{nullptr, vertex, leftArc->rightHalfEdge});
            vertices[leftArc->site->index][2 * static_cast<int>(intersection.side) + 1] = &linkedVertices.back();
            linkedVertices.emplace_back(LinkedVertex{rightArc->leftHalfEdge, vertex, nullptr});
            vertices[rightArc->site->index][2 * static_cast<int>(intersection.side)] = &linkedVertices.back();
            // Next edge
            leftArc = rightArc;
            rightArc = rightArc->next;
        }
    }
    // Add corners
    for (auto& kv : vertices)
    {
        auto& cellVertices = kv.second;
        // We check twice the first side to be sure that all necessary corners are added
        for (std::size_t i = 0; i < 5; ++i)
        {
            std::size_t side = i % 4;
            std::size_t nextSide = (side + 1) % 4;
            // Add first corner
            if (cellVertices[2 * side] == nullptr && cellVertices[2 * side + 1] != nullptr)
            {
                std::size_t prevSide = (side + 3) % 4;
                VoronoiDiagram::Vertex* corner = mDiagram.createCorner(box, static_cast<Boundary::Side>(side));
                linkedVertices.emplace_back(LinkedVertex{nullptr, corner, nullptr});
                cellVertices[2 * prevSide + 1] = &linkedVertices.back();
                cellVertices[2 * side] = &linkedVertices.back();
            }
            // Add second corner
            else if (cellVertices[2 * side] != nullptr && cellVertices[2 * side + 1] == nullptr)
            {
                VoronoiDiagram::Vertex* corner = mDiagram.createCorner(box, static_cast<Boundary::Side>(nextSide));
                linkedVertices.emplace_back(LinkedVertex{nullptr, corner, nullptr});
                cellVertices[2 * side + 1] = &linkedVertices.back();
                cellVertices[2 * nextSide] = &linkedVertices.back();
            }
        }
    }
    // Join the half edges
    for (auto& kv : vertices)
    {
        std::size_t i = kv.first;
        auto& cellVertices = kv.second;
        for (std::size_t side = 0; side < 4; ++side)
        {
            if (cellVertices[2 * side] != nullptr)
            {
                // Link vertices
                VoronoiDiagram::HalfEdge* halfEdge = mDiagram.createHalfEdge(mDiagram.getFace(i));
                halfEdge->origin = cellVertices[2 * side]->vertex;
                halfEdge->destination = cellVertices[2 * side + 1]->vertex;
                cellVertices[2 * side]->nextHalfEdge = halfEdge;
                halfEdge->prev = cellVertices[2 * side]->prevHalfEdge;
                if (cellVertices[2 * side]->prevHalfEdge != nullptr)
                    cellVertices[2 * side]->prevHalfEdge->next = halfEdge;
                cellVertices[2 * side + 1]->prevHalfEdge = halfEdge;
                halfEdge->next = cellVertices[2 * side + 1]->nextHalfEdge;
                if (cellVertices[2 * side + 1]->nextHalfEdge != nullptr)
                    cellVertices[2 * side + 1]->nextHalfEdge->prev = halfEdge;
            }
        }
    }
    return true; // TO DO: detect errors
}
