//
//  svg.cpp
//  Voronoi
//
//  Created by Ayush Tiwari on 29/09/19.
//  Copyright © 2019 Ayush Tiwari. All rights reserved.
//

#include "SVG.h"

/**
    Outputs tag for an SVG Point.
 */
std::string svgPoint(float x, float y, float r) {
    
    std::string out;
    
    out += "<circle cx=\"";
    out += std::to_string(x);
    out += "\" cy=\"";
    out += std::to_string(y);
    out += "\" r=\"2\" stroke=\"brown\" stroke-width=\"0\" fill=\"brown\" />";
    
    return out;
    
}

/**
    Outputs tag for an  SVG Circle.
 */
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

/**
    Outputs tag for an SVG Line.
 */
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
