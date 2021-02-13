//
// Created by lab101 on 28/12/2020.
//

#include "TouchUI.h"
#include "Slider.h"
#include "AssetLoader/AssetsCache.h"
#include "../Helpers/GlobalSettings.h"

using namespace std;
using namespace ci;
using namespace ci::app;
using namespace po::scene;


TouchUIRef TouchUI::create()
{
	TouchUIRef ref = std::make_shared<TouchUI>();
	return ref;
}

float TouchUI::getScale() {
	return 1;//zoomSlider->getSliderValue();
}

float TouchUI::getStrokeScale() {
	return strokeSlider->getSliderValue();
}

ci::Color TouchUI::getColor() {
	return colorPicker->getSelectedColor();
}



void TouchUI::setup(float yOffset) {

	colorPicker = ColorPicker::create();
	colorPicker->setup();
	getView()->addSubview(colorPicker);
	colorPicker->setPosition(10, yOffset + 10);

	strokeSlider = Slider::create();
	strokeSlider->setup();
	strokeSlider->setControlWidth(150);
	getView()->addSubview(strokeSlider);
	strokeSlider->setPosition(10, colorPicker->getFrame().y2 + 80);
	strokeSlider->setSliderPosition(0.5);
	strokeSlider->setShowProgressActive(true);

	scrollBox = ScrollBox::create();
	scrollBox->setup(200, yOffset + 10 + colorPicker->getHeight());
	getView()->addSubview(scrollBox);
	scrollBox->setPosition(10, 520);
	scrollBox->mOnValueChanged.connect([=](int value) {
		onFrameSlected.emit(value);
		});


	scrollBox->setVisible(GS()->nrOfFrames.value() > 1);



	//	zoomSlider = Slider::create();
		//zoomSlider->setup();
		//getView()->addSubview(zoomSlider);
		//zoomSlider->setPosition(10, 1140);
		//zoomSlider->setShowProgressActive(true);
		//zoomSlider->setSliderPosition(0.4);


	mEraseButton = TouchButton::create();
	auto text = CACHE()->getTextureByAssetPath("UI/erase.png");
	mEraseButton->setImage(text);
	getView()->addSubview(mEraseButton);
	mEraseButton->setPosition(360, yOffset + 10);


	mEraseButton->getSignalPressed().connect([=](TouchButtonRef ref) {
		onErase.emit();
		});


	mSaveButton = TouchButton::create();
	auto textSave = CACHE()->getTextureByAssetPath("UI/save.png");
	mSaveButton->setImage(textSave);
	getView()->addSubview(mSaveButton);
	mSaveButton->setPosition(280, yOffset + 10);

	mSaveButton->getSignalPressed().connect([=](TouchButtonRef ref) {
		onSave.emit();
		});


	enableClearButton(!GS()->hideClearButton.value());
	enableSaveButton(!GS()->hideSaveButton.value());



	mLineButton = TouchButton::create();
	auto txtLine = CACHE()->getTextureByAssetPath("UI/line.png");
	mLineButton->setImage(txtLine);
	getView()->addSubview(mLineButton);
	mLineButton->setPosition(240, yOffset + 200);

	mLineButton->getSignalPressed().connect([=](TouchButtonRef ref) {
		onNewToolElection.emit(ToolState::LINE);
		});

	mCircleButton = TouchButton::create();
	auto txtCircle = CACHE()->getTextureByAssetPath("UI/circle.png");
	mCircleButton->setImage(txtCircle);
	getView()->addSubview(mCircleButton);
	mCircleButton->setPosition(240, yOffset + 300);

	mCircleButton->getSignalPressed().connect([=](TouchButtonRef ref) {
		onNewToolElection.emit(ToolState::CIRCLE);
		});

}

void TouchUI::enableClearButton(bool value) {
	mEraseButton->setVisible(value);
	mSaveButton->setOffset(vec2(value ? 0 : 80, 0));
}

void TouchUI::enableSaveButton(bool value) {
	mSaveButton->setVisible(value);
}

void TouchUI::setActiveFrame(int index) {
	scrollBox->setActiveFrame(index);
}

void TouchUI::updateThumbs(std::vector<gl::TextureRef> textures) {
	scrollBox->setTextures(textures);
}


void TouchUI::update() {
}
