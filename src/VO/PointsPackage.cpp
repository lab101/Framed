//
//  PointsPackage.cpp
//  LineProjector2
//
//  Created by Lenn Vh on 12/03/2019.
//

#include "PointsPackage.h"

void PointsPackage::setup(std::vector<ci::vec3> _points, ci::Color _color){
    points = _points;
    color = _color;
    shape = ToolState::BRUSH;
    isEraserOn = false;
    frameId = 0;
};

void PointsPackage::setShape(ToolState _shape){
    shape =_shape;
}

void PointsPackage::setEraser(bool _isEraserOn){
    isEraserOn =_isEraserOn;
}
