//
//  BrushManager.hpp
//  LiveDraw
//
//  Created by lab101 on 06/09/16.
//
//

#ifndef BrushManager_hpp
#define BrushManager_hpp

#include <stdio.h>
#include "Singleton.h"
#include "cinder/Surface.h"
#include "cinder/gl/gl.h"

class BrushManager{
    
    
public:
    ci::gl::TextureRef  dot;
    ci::Surface mBrush;
    ci::gl::GlslProgRef     mShader;
    ci::ColorA  mActiveColor;

    bool isEraserOn;
    float brushScale;
    void setup();
    bool isSetup = false;
//    void generateBrush(float size,float softness);
    void drawBrush(std::vector<ci::vec3>& points,float softness,ci::ColorA color);
};

typedef Singleton<BrushManager> BrushManagerSingleton;


#endif /* BrushManager_hpp */
