//
// Created by lab101 on 28/12/2020.
//

#pragma once

#include "poScene/ViewController.h"
#include "poScene/ImageView.h"
#include "ColorSlider.h"
#include "TouchButton.h"
#include "cinder/Signals.h"
#include "ColorPicker.h"


class TouchUI;
typedef std::shared_ptr<TouchUI> TouchUIRef;

class TouchUI : public po::scene::ViewController {

private:

//	ColorSliderRef colorSlider1;
	SliderRef zoomSlider;
	SliderRef frameSlider;
    ColorPickerRef colorPicker;


	TouchButtonRef mStartButton;

	float mScale;

    std::vector<po::scene::ImageViewRef> mThumbs;
    
    
public:
    
    ci::signals::Signal<void( int )> onFrameSlected;
    
	void setup();
	void update();
	void draw();

	static TouchUIRef create();

	ci::Color getColor();
	float getScale();
	float getFrameScale();
    
    void addThumbs(std::vector<ci::gl::TextureRef> textures);
    void updateThumbs(std::vector<ci::gl::TextureRef> textures);
    void setActiveFrame(int index);


};

