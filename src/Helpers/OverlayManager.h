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
    bool isWebcamStarted = false;
	ci::ivec2 mSize;
    
    ci::CaptureRef            mCapture;
    
    std::vector<ci::fs::path> mOverlayFolders;
    int selectedOverlayFolder = 0;
    
public:
    bool isLive = false;

	void setup(int nrOfFrames, ci::vec2 size);
    void update();
    void setupCamera();
    void drawAtIndex(int index);
    void snap();

    void clearAll();
    void setActiveFrame(int id);
    void nextFrame();
    void prevFrame();
    
    void loadOverlayFolders();
    void loadOverlay(ci::fs::path path);

    int getActiveFrame();

	ci::ivec2 getSize();
	void drawGUI();
    
    
    std::vector<ci::gl::TextureRef> getTextures();
    

};

