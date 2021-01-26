//
//  PointsPack.hpp
//  LiveDraw
//
//  Created by lab101 on 06/09/16.
//
//

#pragma once

#include <stdio.h>
#include "cinder/Vector.h"
#include <vector>
#include "cinder/color.h"

class PointsPackage{
    
    
    
public:
    std::vector<ci::vec3> points;
    ci::Color color;
    std::string shape;
    bool isEraserOn;
    int frameId;
    
    void setup(std::vector<ci::vec3> points, ci::Color color);
    void setShape(std::string shape);
    void setEraser(bool isEraserOn);
};




