//
// Created by lab101 on 20/01/2021.
//

#include "LineManager.h"
#include "cinder/app/App.h"

using  namespace  ci;
using namespace std;


void LineManager::newLine(ci::vec3 pressurePoint){

    clearPath();

    mPath.moveTo(vec2(pressurePoint.x   ,pressurePoint.y));
    mDepths.moveTo(vec2(pressurePoint.x ,pressurePoint.z));
    std::cout << pressurePoint.x << " - " << pressurePoint.y << std::endl;

}


void LineManager::endLine(){


//    if(GS()->hasGifOutput.value()){
//        saveLineSegmentForGif();
//    }

   // mStepId++;
    clearPath();
}


void LineManager::clearPath(){

    mPath.clear();
    mDepths.clear();

//    pointVec newStrokes;
//    strokes.push_back(newStrokes);

    lastDrawDistance = 0;
    minDistance = 0;

}

void LineManager::lineTo(ci::vec3 pressurePoint,ci::ColorA color){
    //if(!isInsideComp(pressurePoint)) return;

    //std::cout << pressurePoint.x << " - " << pressurePoint.y << std::endl;

    if(mPath.empty()){
        newLine(pressurePoint);
    }else{
        mPath.lineTo(vec2(pressurePoint.x,pressurePoint.y));
        mDepths.lineTo(vec2(pressurePoint.x,pressurePoint.z));
        calculatePath(mPath,mDepths,true,color);
    }
}


void LineManager::calculatePath(ci::Path2d& path,ci::Path2d& depths, bool emmitTrueOrFalse, ci::ColorA color){

    mSize = ci::app::getWindowSize();

    float length = path.calcLength();
    if(length <= minDistance) return;

    float newDrawPosition = lastDrawDistance + minDistance;

    pointVec pointsToDraw;
    pointVec pointsToDrawNormalised;

    while(newDrawPosition + minDistance < length){

        float newTime = path.calcTimeForDistance(newDrawPosition);
        if(newDrawPosition == 0) newTime = 0;

        vec3 newPoint(path.getPosition(newTime),depths.getPosition(newTime).y);

        pointsToDraw.push_back(newPoint);

        minDistance = fmax(2.8f,(newPoint.z * .17));

        lastDrawDistance = newDrawPosition;
        newDrawPosition = (lastDrawDistance + minDistance);

        // save them normalised later to a file.
        newPoint.x /= mSize.x;
        newPoint.y /= mSize.y;

        pointsToDrawNormalised.push_back(newPoint);
      //  strokes.back().push_back(newPoint);

    }

    if(pointsToDraw.size() > 0 ){
        // emmit to other listner in this case network
        //if(emmitTrueOrFalse)
        //
        onNewPoints.emit(pointsToDrawNormalised);
        // draw the new points into the fbo.
        //drawInFbo(pointsToDraw,color);
    }
}
