//
//  BeachTree.cpp
//  Voronoi
//
//  Created by Ayush Tiwari on 28/09/19.
//  Copyright © 2019 Ayush Tiwari. All rights reserved.
//

#include "BeachTree.h"

#include <limits>
#include <cmath>

#include "BeachElement.h"

BeachTree::BeachTree() : mNil(new BeachElement), mRoot(mNil)
{
    mNil->color = BeachElement::Color::BLACK;
}

BeachTree::~BeachTree()
{
    free(mRoot);
    delete mNil;
}

BeachElement* BeachTree::createArc(VoronoiDiagram::Site* site)
{
    return new BeachElement{mNil, mNil, mNil, site, nullptr, nullptr, nullptr, mNil, mNil, BeachElement::Color::RED};
}

bool BeachTree::isEmpty() const
{
    return isNil(mRoot);
}

bool BeachTree::isNil(const BeachElement* x) const
{
    return x == mNil;
}

void BeachTree::setRoot(BeachElement* x)
{
    mRoot = x;
    mRoot->color = BeachElement::Color::BLACK;
}

BeachElement* BeachTree::getLeftmostArc() const
{
    BeachElement* x = mRoot;
    while (!isNil(x->prev))
        x = x->prev;
    return x;
}

BeachElement* BeachTree::locateArcAbove(const EuclidVec& point, double l) const
{
    BeachElement* node = mRoot;
    bool found = false;
    while (!found)
    {
        double breakpointLeft = -std::numeric_limits<double>::infinity();
        double breakpointRight = std::numeric_limits<double>::infinity();
        if (!isNil(node->prev))
           breakpointLeft =  computeBreakpoint(node->prev->site->point, node->site->point, l);
        if (!isNil(node->next))
            breakpointRight = computeBreakpoint(node->site->point, node->next->site->point, l);
        if (point.x < breakpointLeft)
            node = node->left;
        else if (point.x > breakpointRight)
            node = node->right;
        else
            found = true;
    }
    return node;
}

void BeachTree::insertBefore(BeachElement* x, BeachElement* y)
{
    // Find the right place
    if (isNil(x->left))
    {
        x->left = y;
        y->parent = x;
    }
    else
    {
        x->prev->right = y;
        y->parent = x->prev;
    }
    // Set the pointers
    y->prev = x->prev;
    if (!isNil(y->prev))
        y->prev->next = y;
    y->next = x;
    x->prev = y;
    // Balance the tree
    insertFixup(y);
}

void BeachTree::insertAfter(BeachElement* x, BeachElement* y)
{
    // Find the right place
    if (isNil(x->right))
    {
        x->right = y;
        y->parent = x;
    }
    else
    {
        x->next->left = y;
        y->parent = x->next;
    }
    // Set the pointers
    y->next = x->next;
    if (!isNil(y->next))
        y->next->prev = y;
    y->prev = x;
    x->next = y;
    // Balance the tree
    insertFixup(y);
}

void BeachTree::replace(BeachElement* x, BeachElement* y)
{
    transplant(x, y);
    y->left = x->left;
    y->right = x->right;
    if (!isNil(y->left))
        y->left->parent = y;
    if (!isNil(y->right))
        y->right->parent = y;
    y->prev = x->prev;
    y->next = x->next;
    if (!isNil(y->prev))
        y->prev->next = y;
    if (!isNil(y->next))
        y->next->prev = y;
    y->color = x->color;
}

void BeachTree::remove(BeachElement* z)
{
    BeachElement* y = z;
    BeachElement::Color yOriginalColor = y->color;
    BeachElement* x;
    if (isNil(z->left))
    {
        x = z->right;
        transplant(z, z->right);
    }
    else if (isNil(z->right))
    {
        x = z->left;
        transplant(z, z->left);
    }
    else
    {
        y = minimum(z->right);
        yOriginalColor = y->color;
        x = y->right;
        if (y->parent == z)
            x->parent = y; // Because x could be Nil
        else
        {
            transplant(y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        transplant(z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }
    if (yOriginalColor == BeachElement::Color::BLACK)
        removeFixup(x);
    // Update next and prev
    if (!isNil(z->prev))
        z->prev->next = z->next;
    if (!isNil(z->next))
        z->next->prev = z->prev;
}

std::ostream& BeachTree::print(std::ostream& os) const
{
    //return printArc(os, mRoot);
    BeachElement* arc = getLeftmostArc();
    while (!isNil(arc))
    {
        os << arc->site->index << ' ';
        arc = arc->next;
    }
    return os;
}

BeachElement* BeachTree::minimum(BeachElement* x) const
{
    while (!isNil(x->left))
        x = x->left;
    return x;
}

void BeachTree::transplant(BeachElement* u, BeachElement* v)
{
    if (isNil(u->parent))
        mRoot = v;
    else if (u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;
    v->parent = u->parent;
}

void BeachTree::insertFixup(BeachElement* z)
{
    while (z->parent->color == BeachElement::Color::RED)
    {
        if (z->parent == z->parent->parent->left)
        {
            BeachElement* y = z->parent->parent->right;
            // Case 1
            if (y->color == BeachElement::Color::RED)
            {
                z->parent->color = BeachElement::Color::BLACK;
                y->color = BeachElement::Color::BLACK;
                z->parent->parent->color = BeachElement::Color::RED;
                z = z->parent->parent;
            }
            else
            {
                // Case 2
                if (z == z->parent->right)
                {
                    z = z->parent;
                    leftRotate(z);
                }
                // Case 3
                z->parent->color = BeachElement::Color::BLACK;
                z->parent->parent->color = BeachElement::Color::RED;
                rightRotate(z->parent->parent);
            }
        }
        else
        {
            BeachElement* y = z->parent->parent->left;
            // Case 1
            if (y->color == BeachElement::Color::RED)
            {
                z->parent->color = BeachElement::Color::BLACK;
                y->color = BeachElement::Color::BLACK;
                z->parent->parent->color = BeachElement::Color::RED;
                z = z->parent->parent;
            }
            else
            {
                // Case 2
                if (z == z->parent->left)
                {
                    z = z->parent;
                    rightRotate(z);
                }
                // Case 3
                z->parent->color = BeachElement::Color::BLACK;
                z->parent->parent->color = BeachElement::Color::RED;
                leftRotate(z->parent->parent);
            }
        }
    }
    mRoot->color = BeachElement::Color::BLACK;
}

void BeachTree::removeFixup(BeachElement* x)
{

    while (x != mRoot && x->color == BeachElement::Color::BLACK)
    {
        BeachElement* w;
        if (x == x->parent->left)
        {
            w = x->parent->right;
            // Case 1
            if (w->color == BeachElement::Color::RED)
            {
                w->color = BeachElement::Color::BLACK;
                x->parent->color = BeachElement::Color::RED;
                leftRotate(x->parent);
                w = x->parent->right;
            }
            // Case 2
            if (w->left->color == BeachElement::Color::BLACK && w->right->color == BeachElement::Color::BLACK)
            {
                w->color = BeachElement::Color::RED;
                x = x->parent;
            }
            else
            {
                // Case 3
                if (w->right->color == BeachElement::Color::BLACK)
                {
                    w->left->color = BeachElement::Color::BLACK;
                    w->color = BeachElement::Color::RED;
                    rightRotate(w);
                    w = x->parent->right;
                }
                // Case 4
                w->color = x->parent->color;
                x->parent->color = BeachElement::Color::BLACK;
                w->right->color = BeachElement::Color::BLACK;
                leftRotate(x->parent);
                x = mRoot;
            }
        }
        else
        {
            w = x->parent->left;
            // Case 1
            if (w->color == BeachElement::Color::RED)
            {
                w->color = BeachElement::Color::BLACK;
                x->parent->color = BeachElement::Color::RED;
                rightRotate(x->parent);
                w = x->parent->left;
            }
            // Case 2
            if (w->left->color == BeachElement::Color::BLACK && w->right->color == BeachElement::Color::BLACK)
            {
                w->color = BeachElement::Color::RED;
                x = x->parent;
            }
            else
            {
                // Case 3
                if (w->left->color == BeachElement::Color::BLACK)
                {
                    w->right->color = BeachElement::Color::BLACK;
                    w->color = BeachElement::Color::RED;
                    leftRotate(w);
                    w = x->parent->left;
                }
                // Case 4
                w->color = x->parent->color;
                x->parent->color = BeachElement::Color::BLACK;
                w->left->color = BeachElement::Color::BLACK;
                rightRotate(x->parent);
                x = mRoot;
            }
        }
    }
    x->color = BeachElement::Color::BLACK;
}

void BeachTree::leftRotate(BeachElement* x)
{
    BeachElement* y = x->right;
    x->right = y->left;
    if (!isNil(y->left))
        y->left->parent = x;
    y->parent = x->parent;
    if (isNil(x->parent))
        mRoot = y;
    else if (x->parent->left == x)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left = x;
    x->parent = y;
}

void BeachTree::rightRotate(BeachElement* y)
{
    BeachElement* x = y->left;
    y->left = x->right;
    if (!isNil(x->right))
        x->right->parent = y;
    x->parent = y->parent;
    if (isNil(y->parent))
        mRoot = x;
    else if (y->parent->left == y)
        y->parent->left = x;
    else
        y->parent->right = x;
    x->right = y;
    y->parent = x;
}

double BeachTree::computeBreakpoint(const EuclidVec& point1, const EuclidVec& point2, double l) const
{
    double x1 = point1.x, y1 = point1.y, x2 = point2.x, y2 = point2.y;
    double d1 = 1.0 / (2.0 * (y1 - l));
    double d2 = 1.0 / (2.0 * (y2 - l));
    double a = d1 - d2;
    double b = 2.0 * (x2 * d2 - x1 * d1);
    double c = (y1 * y1 + x1 * x1 - l * l) * d1 - (y2 * y2 + x2 * x2 - l * l) * d2;
    double delta = b * b - 4.0 * a * c;
    return (-b + std::sqrt(delta)) / (2.0 * a);
}

void BeachTree::free(BeachElement* x)
{
    if (isNil(x))
        return;
    else
    {
        free(x->left);
        free(x->right);
        delete x;
    }
}

std::ostream& BeachTree::printArc(std::ostream& os, const BeachElement* arc, std::string tabs) const
{
    os << tabs << arc->site->index << ' ' << arc->leftHalfEdge << ' ' << arc->rightHalfEdge << std::endl;
    if (!isNil(arc->left))
        printArc(os, arc->left, tabs + '\t');
    if (!isNil(arc->right))
        printArc(os, arc->right, tabs + '\t');
    return os;
}

std::ostream& operator<<(std::ostream& os, const BeachTree& beachline)
{
    return beachline.print(os);
}
