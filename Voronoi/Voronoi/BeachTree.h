//
//  BeachTree.h
//  Voronoi
//
//  Created by Ayush Tiwari on 28/09/19.
//  Copyright © 2019 Ayush Tiwari. All rights reserved.
//

#pragma once

#include "EuclidVec.h"
#include "VoronoiDiagram.h"

class BeachElement;

/// RB Tree for Storing the Beach Line.
class BeachTree
{
public:
    BeachTree();
    ~BeachTree();

    // Remove copy and move operations
    BeachTree(const BeachTree&) = delete;
    BeachTree& operator=(const BeachTree&) = delete;
    BeachTree(BeachTree&&) = delete;
    BeachTree& operator=(BeachTree&&) = delete;

    BeachElement* createArc(VoronoiDiagram::Site* site);

    bool isEmpty() const;
    bool isNil(const BeachElement* x) const;
    void setRoot(BeachElement* x);
    BeachElement* getLeftmostArc() const;

    BeachElement* locateArcAbove(const EuclidVec& point, double l) const;
    void insertBefore(BeachElement* x, BeachElement* y);
    void insertAfter(BeachElement* x, BeachElement* y);
    void replace(BeachElement* x, BeachElement* y);
    void remove(BeachElement* z);

    std::ostream& print(std::ostream& os) const;

private:
    BeachElement* mNil;
    BeachElement* mRoot;

    // Utility methods
    BeachElement* minimum(BeachElement* x) const;
    void transplant(BeachElement* u, BeachElement* v);

    // Fixup functions
    void insertFixup(BeachElement* z);
    void removeFixup(BeachElement* x);

    // Rotations
    void leftRotate(BeachElement* x);
    void rightRotate(BeachElement* y);

    double computeBreakpoint(const EuclidVec& point1, const EuclidVec& point2, double l) const;

    void free(BeachElement* x);

    std::ostream& printArc(std::ostream& os, const BeachElement* arc, std::string tabs = "") const;
};

std::ostream& operator<<(std::ostream& os, const BeachTree& beachline);
