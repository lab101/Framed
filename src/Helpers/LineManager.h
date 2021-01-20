//
// Created by lab101 on 20/01/2021.
//

#pragma once

#include "cinder/Path2d.h"
#include "cinder/Color.h"
#include "cinder/Signals.h"

typedef std::vector<ci::vec3> pointVec;

class LineManager {

private:
    ci::vec2    mSize;

    ci::Path2d mPath;
    ci::Path2d mDepths;

    float lastDrawDistance      = 0;
    float minDistance           = 0;
    void calculatePath(ci::Path2d& path,ci::Path2d& depths, bool emmitTrueOrFalse,ci::ColorA color);

    void clearPath();

public:


    ci::signals::Signal<void(pointVec)>   onNewPoints;


    void newLine(ci::vec3 pressurePoint);
    void lineTo(ci::vec3 pressurePoint,ci::ColorA color);
    void endLine();
};

