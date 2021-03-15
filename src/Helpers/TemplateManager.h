//
// Created by lab101 on 21/01/2021.
//

#pragma once

#include "Frame.h"
#include "cinder/gl/Texture.h"
#include "cinder/Capture.h"

class TemplateManager {

	std::vector<ci::gl::TextureRef> mFrames;
	ci::ivec2 mSize;
    
public:

	void setup();
    void update();
    void drawAtIndex(int index);
    void clearAll();
    void loadTemplate(ci::fs::path path);

	void drawGUI();
    
    std::vector<ci::gl::TextureRef> getTextures();
    

};

