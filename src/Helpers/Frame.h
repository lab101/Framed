//
// Created by lab101 on 21/01/2021.
//

#include "cinder/gl/gl.h"


#pragma once

class Frame {
    ci::gl::FboRef              mActiveFbo;

    void activateFbo();

public:
    void setup(ci::vec2 size);
    void drawPoints(std::vector<ci::vec3>& points,ci::Color color);
    void drawCircle(ci::vec2 point1, ci::vec2 point2, ci::Color color);

    void draw();
    ci::gl::Texture2dRef getTexture();

    void clearFbo();
    void setFbo(ci::gl::FboRef& fbo,ci::ivec2 size,float windowScale);
    void writeBuffer(std::string path);

};
