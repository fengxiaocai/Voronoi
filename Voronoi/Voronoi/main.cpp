//
//  main.cpp
//  Voronoi
//
//  Created by Ayush Tiwari on 28/09/19.
//  Copyright © 2019 Ayush Tiwari. All rights reserved.
//

// STL
#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <fstream>
#include <string>
#include <cmath>

#include "FortuneAlgorithm.h"

const float SCALE = 700.0;

std::vector<Vector2> generatePoints(int nbPoints)
{
    uint64_t seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::cout << "seed: " << seed << '\n';
    std::default_random_engine generator(seed);
    std::uniform_real_distribution<double> distribution (0.15, 0.85);

    std::vector<Vector2> points;
    for (int i = 0; i < nbPoints; ++i)
        points.push_back(Vector2{distribution(generator), distribution(generator)});

    return points;
}

VoronoiDiagram generateRandomDiagram(std::size_t nbPoints)
{
    // Generate points
    std::vector<Vector2> points = generatePoints(nbPoints);

    // Construct diagram
    FortuneAlgorithm algorithm(points);
    auto start = std::chrono::steady_clock::now();
    algorithm.construct();
    auto duration = std::chrono::steady_clock::now() - start;
    std::cout << "construction: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << '\n';

    // Bound the diagram
    start = std::chrono::steady_clock::now();
    algorithm.bound(Box{-0.05, -0.05, 1.05, 1.05}); // Take the bounding box slightly bigger than the intersection box
    duration = std::chrono::steady_clock::now() - start;
    std::cout << "bounding: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << '\n';
    VoronoiDiagram diagram = algorithm.getDiagram();

    // Intersect the diagram with a box
    start = std::chrono::steady_clock::now();
    bool valid = diagram.intersect(Box{0.0, 0.0, 1.0, 1.0});
    duration = std::chrono::steady_clock::now() - start;
    std::cout << "intersection: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << '\n';
    if (!valid)
        throw std::runtime_error("An error occured in the box intersection algorithm");

    return diagram;
}

float pointToLine(float x1, float y1, float x2, float y2, float px, float py) {
    
    float A = (y1-y2)/(x1-x2);
    float B = -1;
    float C = y1 - A * x1;
    
    return fabs((A*px + B*py + C)/sqrt(A*A + B*B));
    
}

std::string svgPoint(float x, float y, float r) {
    
    std::string out;
    
    out += "<circle cx=\"";
    out += std::to_string(x);
    out += "\" cy=\"";
    out += std::to_string(y);
    out += "\" r=\"2\" stroke=\"brown\" stroke-width=\"0\" fill=\"brown\" />";
    
    return out;
    
}

std::string svgBoundary(float x, float y, float r) {
    
    std::string out;
    
    out += "<circle cx=\"";
    out += std::to_string(x);
    out += "\" cy=\"";
    out += std::to_string(y);
    out += "\" r=\"";
    out += std::to_string(r);
    out += "\" stroke=\"blue\" stroke-width=\"1\" fill=\"yellow\" fill-opacity=\"0.4\" />";
    
    return out;
    
}

std::string svgLine(float x1, float y1, float x2, float y2) {
    std::string out;
    
    out += "<line x1=\"";
    out += std::to_string(x1);
    out += "\" y1=\"";
    out += std::to_string(y1);
    out += "\" x2=\"";
    out += std::to_string(x2);
    out += "\" y2=\"";
    out += std::to_string(y2);
    out += "\" stroke=\"red\" stroke-width=\"1\" />";
    
    return out;
}

void drawDiagram(VoronoiDiagram& diagram)
{
    std::cout << "Genterating Edges\n\n";
    
    std::ofstream file;
    file.open("output.svg");
    
    file << "<svg xmlns=\"http://www.w3.org/2000/svg\">" << std::endl;
    
    for (std::size_t i = 0; i < diagram.getNbSites(); ++i)
    {
        std::cout << "\n\n";
        const VoronoiDiagram::Site* site = diagram.getSite(i);
        Vector2 center = site->point;
        VoronoiDiagram::Face* face = site->face;
        VoronoiDiagram::HalfEdge* halfEdge = face->outerComponent;

        file << svgPoint(center.x * SCALE, center.y * SCALE, 2) << std::endl;

        if (halfEdge == nullptr)
            continue;
        while (halfEdge->prev != nullptr)
        {
            halfEdge = halfEdge->prev;
            if (halfEdge == face->outerComponent)
                break;
        }
        VoronoiDiagram::HalfEdge* start = halfEdge;

        std::cout << center*SCALE << std::endl;

        std::cout << "Surrounding Edges : " << std::endl;
        
        float minDist = 600.0;
        while (halfEdge != nullptr)
        {
            if (halfEdge->origin != nullptr && halfEdge->destination != nullptr)
            {
                Vector2 origin = (halfEdge->origin->point) * SCALE;
                Vector2 destination = (halfEdge->destination->point) * SCALE;

                std::cout << origin << " " << destination << std::endl;
                
                file << svgLine(origin.x, origin.y, destination.x, destination.y) << std::endl;
                
                float currDist = pointToLine(origin.x, origin.y, destination.x, destination.y, center.x * SCALE, center.y * SCALE);
                
                if(currDist < minDist) minDist = currDist;
            }
            halfEdge = halfEdge->next;
            if (halfEdge == start)
                break;
        }
        
        file << svgBoundary(center.x * SCALE, center.y * SCALE, minDist) << std::endl;
    }
    
    file << "</svg>" << std::endl;
    
    file.close();
}




int main(int argc, const char * argv[]) {

    std::size_t nbPoints = 50;
    VoronoiDiagram diagram = generateRandomDiagram(nbPoints);
    drawDiagram(diagram);
    
    std::cout << "\n\n";
    std::cout << pointToLine(0, 2, -1.66, 0, -3, 7);
    
    return 0;
}
