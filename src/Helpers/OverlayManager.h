//
// Created by lab101 on 21/01/2021.
//

#pragma once

#include "Frame.h"
#include "cinder/gl/Texture.h"
#include "cinder/Capture.h"

class OverlayManager {

	std::vector<ci::gl::TextureRef> mFrames;
	int mActiveFrameIndex;
	ci::ivec2 mSize;
    
    ci::CaptureRef            mCapture;


public:

	void setup(int nrOfFrames, ci::vec2 size);
    void setupCamera();
    void drawAtIndex(int index);
    void snap();

    void clearAll();
    void setActiveFrame(int id);
    void nextFrame();
    void prevFrame();

    int getActiveFrame();

	ci::ivec2 getSize();
	void drawGUI();
    
    
    std::vector<ci::gl::TextureRef> getTextures();
    

};

