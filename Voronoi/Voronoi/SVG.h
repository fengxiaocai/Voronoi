//
//  svg.h
//  Voronoi
//
//  Created by Ayush Tiwari on 29/09/19.
//  Copyright © 2019 Ayush Tiwari. All rights reserved.
//

#pragma once

#include <string>

std::string svgPoint(float x, float y, float r);

std::string svgBoundary(float x, float y, float r);

std::string svgLine(float x1, float y1, float x2, float y2);
