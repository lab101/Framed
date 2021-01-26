//
// Created by lab101 on 21/01/2021.
//

#pragma once

#include "Frame.h"

class FrameManager {

	std::vector<Frame> mFrames;
	int mActiveFrameIndex;
	float mFrameSpeed = 0.5;
	ci::ivec2 mSize;

public:

	void setup(int nrOfFrames, ci::vec2 size);
	void draw();
    void drawAtIndex(int index);

    void clearAll();
    void saveAll();
	void drawLoop();
	void drawPoints(std::vector<ci::vec3>& points, ci::Color color,int frameId= -1);
	void setFrameIndexNormalised(float value);
    void setActiveFrame(int id);
    int getActiveFrame();

	ci::ivec2 getSize();
	void drawGUI();
    
    
    std::vector<ci::gl::TextureRef> getTextures();

    

};

