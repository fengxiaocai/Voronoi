//
//  main.cpp
//  Voronoi
//
//  Created by Ayush Tiwari on 28/09/19.
//  Copyright © 2019 Ayush Tiwari. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <cmath>

#include "Fortune.h"
#include "Utilities.h"
#include "SVG.h"

const float SCALE = 1000.0;

/**
    Calculates the distance between a point and a line.
 */
float pointToLine(float x1, float y1, float x2, float y2, float px, float py) {
    
    float A = (y1-y2)/(x1-x2);
    float B = -1;
    float C = y1 - A * x1;
    
    return fabs((A*px + B*py + C)/sqrt(A*A + B*B));
    
}

/**
    Outputs the Voronoi Diagram on an SVG File.
 */
void drawDiagram(VoronoiDiagram& diagram)
{
    std::cout << "Genterating Edges\n\n";
    
    std::ofstream file;
    file.open("output.svg");
    
    file << "<svg xmlns=\"http://www.w3.org/2000/svg\">" << std::endl;
    
    for (std::size_t i = 0; i < diagram.getSitesCount(); ++i)
    {
        std::cout << "\n\n";
        const VoronoiDiagram::Site* site = diagram.getSite(i);
        EuclidVec sitePoint = site->point;
        VoronoiDiagram::Face* face = site->face;
        VoronoiDiagram::HalfEdge* innerEdge = face->innerHalfEdge;

        file << svgPoint(sitePoint.x * SCALE, sitePoint.y * SCALE, 2) << std::endl;

        if (innerEdge == nullptr)
            continue;
        while (innerEdge->prev != nullptr)
        {
            innerEdge = innerEdge->prev;
            if (innerEdge == face->innerHalfEdge)
                break;
        }
        
        VoronoiDiagram::HalfEdge* start = innerEdge;

        std::cout << sitePoint*SCALE << std::endl;

        std::cout << "Surrounding Edges : " << std::endl;
        
        float minDist = 600.0;
        while (innerEdge != nullptr)
        {
            if (innerEdge->origin != nullptr && innerEdge->destination != nullptr)
            {
                EuclidVec pointA = (innerEdge->origin->point) * SCALE;
                EuclidVec pointB = (innerEdge->destination->point) * SCALE;

                std::cout << pointA << " " << pointB << std::endl;
                
                file << svgLine(pointA.x, pointA.y, pointB.x, pointB.y) << std::endl;
                
                float currDist = pointToLine(pointA.x, pointA.y, pointB.x, pointB.y, sitePoint.x * SCALE, sitePoint.y * SCALE);
                
                if(currDist < minDist) minDist = currDist;
            }
            innerEdge = innerEdge->next;
            if (innerEdge == start)
                break;
        }
        
        file << svgBoundary(sitePoint.x * SCALE, sitePoint.y * SCALE, minDist) << std::endl;
    }
    
    file << "</svg>" << std::endl;
    
    file.close();
}

/**
    Driver Function.
 */
int main(int argc, const char * argv[]) {

    std::size_t nbPoints = 100;
    VoronoiDiagram dcel = generateRandomDiagram(nbPoints);
    drawDiagram(dcel);
    
    return 0;
}
