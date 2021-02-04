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

	return zoomSlider->getSliderValue();
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


	zoomSlider = Slider::create();
	zoomSlider->setup();
	//getView()->addSubview(zoomSlider);
	zoomSlider->setPosition(10, 1140);
	zoomSlider->setShowProgressActive(true);
	zoomSlider->setSliderPosition(0.4);





	mEraseButton = TouchButton::create();
	auto text = CACHE()->getTextureByAssetPath("UI/erase.png");
	mEraseButton->setImage(text);
	getView()->addSubview(mEraseButton);
	mEraseButton->setPosition(356, yOffset + 10);


	mEraseButton->getSignalPressed().connect([=](TouchButtonRef ref) {
		onErase.emit();
		});



}


void TouchUI::setActiveFrame(int index) {
	onFrameSlected.emit(index);

	for (auto thumb : mThumbs) {
		thumb->setAlpha(0.18);
	}
	mThumbs[index]->setAlpha(1);

}


void TouchUI::addThumbs(std::vector<gl::TextureRef> textures) {

	for (int i = 0; i < textures.size(); i++) {
		po::scene::ImageViewRef img = ImageView::create(textures[i]);
		img->setScale(0.1);
		const float margin = 6;
		img->setPosition(10, 520 + ((img->getScaledHeight() + margin) * i));
		mThumbs.push_back(img);

		img->getSignal(po::scene::MouseEvent::DOWN_INSIDE).connect([=](po::scene::MouseEvent& event) {
			setActiveFrame(i);
			});

	}
}


void TouchUI::updateThumbs(std::vector<gl::TextureRef> textures) {
	scrollBox->setTextures(textures);
}


void TouchUI::update() {
}