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
#include "cinder/Color.h"
#include "../enum.h"

class PointsPackage{


public:
    std::vector<ci::vec3> points;
    ci::Color color;
    ToolState shape;
    bool isEraserOn;
    int frameId;
    
    void setup(std::vector<ci::vec3> points, ci::Color color);
    void setShape(ToolState shape);
    void setEraser(bool isEraserOn);
};




