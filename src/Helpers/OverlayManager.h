//
// Created by lab101 on 21/01/2021.
//

#pragma once

#include "Frame.h"
#include "cinder/gl/Texture.h"
#include "cinder/Capture.h"


class OverlayFrame {

public:

	ci::gl::TextureRef mTexture;
	bool flipHorizontal = false;

	OverlayFrame(ci::gl::TextureRef texture, bool isFlipped = false) {
		mTexture = texture;
		flipHorizontal = isFlipped;
	}
};

class OverlayManager {

	std::map<int,ci::gl::TextureRef> mFrames;

	int mActiveFrameIndex;
	bool isWebcamStarted = false;
	ci::ivec2 mSize;

	ci::CaptureRef          mCapture = nullptr;
	int						mSelectedWebcam;

	std::vector<ci::fs::path> mOverlayFolders;
	std::vector<std::string> webcamList;
	std::vector<ci::Capture::DeviceRef> deviceList;
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
    void setTexture(int index, ci::gl::TextureRef texture);

    int getActiveFrame();

	ci::ivec2 getSize();
	void drawGUI();
    
    std::vector<std::string> getWebcamList();

    
    //std::map<int ,ci::gl::TextureRef> getTextures();


};

