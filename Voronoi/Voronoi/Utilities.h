//
//  Utilities.h
//  Voronoi
//
//  Created by Ayush Tiwari on 29/09/19.
//  Copyright © 2019 Ayush Tiwari. All rights reserved.
//

#pragma once

#include <iostream>
#include <vector>
#include <chrono>
#include <random>

#include "Fortune.h"

std::vector<EuclidVec> generatePoints(int n);

VoronoiDiagram generateRandomDiagram(std::size_t nbPoints);
