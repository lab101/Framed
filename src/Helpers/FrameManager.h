//
// Created by lab101 on 21/01/2021.
//

#pragma once

#include "Frame.h"

#ifdef CINDER_MAC
#include "syphonServer.h"
#endif


class FrameManager {

	std::vector<Frame*> mFrames;
	int mActiveFrameIndex;
	ci::ivec2 mSize;
    
    syphonServer mTextureSyphon;
    //each item to publish requires a different server


public:

	int totalFrameCount();
	void setup(int nrOfFrames, ci::vec2 size);
	void draw(bool isFullscreen = false);
	void drawAtIndex(int index);

	void clearAll();
	void drawTextures(std::vector<ci::gl::TextureRef> textures);

	void saveAll();
	void drawLoop(bool isFullscreen = false);
	void drawPoints(std::vector<ci::vec3>& points, ci::Color color, int frameId = 0);
	void drawCircle(ci::vec2 p1, ci::vec2 p2, ci::Color color, int frameId = 0);
	void drawRectangle(ci::vec2 p1, ci::vec2 p2, ci::Color color, int frameId = 0);

	void setActiveFrame(int id);
	void nextFrame();
	void prevFrame();

	int getActiveFrame();
	ci::gl::TextureRef getActiveTexture();
	ci::gl::TextureRef getLoopTexture();

	void changeNrOfFrames(int nrOfFrames);

	ci::ivec2 getSize();
	void drawGUI();


	std::vector<ci::gl::TextureRef> getTextures();



};

