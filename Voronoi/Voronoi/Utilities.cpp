//
//  Utilites.cpp
//  Voronoi
//
//  Created by Ayush Tiwari on 29/09/19.
//  Copyright © 2019 Ayush Tiwari. All rights reserved.
//


#include "Utilities.h"

/**
   Function to generate random points.
*/
std::vector<EuclidVec> generatePoints(int n)
{
    uint64_t seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::cout << "seed: " << seed << '\n';
    
    std::default_random_engine generator(seed);
    
    std::uniform_real_distribution<double> distribution1 (0.10, 0.60);
    std::uniform_real_distribution<double> distribution2 (0.10, 0.90);
    
    std::vector<EuclidVec> points;
    
    for (int i = 0; i < n; ++i)
        points.push_back(EuclidVec{distribution2(generator), distribution1(generator)});

    return points;
}

/**
    Builds the Voronoi Diagram using Fortunes Algorithm
 */
VoronoiDiagram generateRandomDiagram(std::size_t nbPoints)
{
    // Generate points
    std::vector<EuclidVec> points = generatePoints(nbPoints);

    // Construct diagram
    Fortune algorithm(points);
    
    algorithm.build();

    // Bound the diagram
    
    algorithm.bound(Boundary{-0.05, -0.05, 1.05, 1.05}); // Take the bounding box slightly bigger than the intersection box
    VoronoiDiagram diagram = algorithm.getDiagram();

    // Intersect the diagram with a box
    bool valid = diagram.intersect(Boundary{0.0, 0.0, 1.0, 1.0});
    
    if (!valid)
        throw std::runtime_error("An error occured in the box intersection algorithm");

    return diagram;
}

