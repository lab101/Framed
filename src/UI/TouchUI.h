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
#include "ScrollBox.h"
#include "../enum.h"

class TouchUI;
typedef std::shared_ptr<TouchUI> TouchUIRef;

class TouchUI : public po::scene::ViewController {

private:

	//SliderRef zoomSlider;
	SliderRef strokeSlider;
	ColorPickerRef colorPicker;
	ScrollBoxRef scrollBox;

	TouchButtonRef mEraseButton;
	TouchButtonRef mSaveButton;

	TouchButtonRef mLineButton;
	TouchButtonRef mCircleButton;
	TouchButtonRef mRectButton;

public:

	ci::signals::Signal<void(int)> onFrameSlected;
	ci::signals::Signal<void()> onErase;
	ci::signals::Signal<void()> onSave;
	ci::signals::Signal<void(ToolState)> onNewToolElection;

	void setup(float yOffset);
	void update();

	static TouchUIRef create();

	ci::Color getColor();
	float getScale();
	float getStrokeScale();

	//void addThumbs(std::vector<ci::gl::TextureRef> textures);
	void updateThumbs(std::vector<ci::gl::TextureRef> textures);
	void setActiveFrame(int index);
	void enableClearButton(bool value);
	void enableSaveButton(bool value);

};

