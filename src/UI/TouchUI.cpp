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

float TouchUI::getFrameScale() {

	return frameSlider->getSliderValue();
}

ci::Color TouchUI::getColor() {

	float hue = colorSlider1->getSliderValue();
	auto color = Color(CM_HSV, vec3(hue, 0.8, 0.8));
	return color;
}





void TouchUI::setup() {

	colorSlider1 = ColorSlider::create();
	colorSlider1->setup();
	getView()->addSubview(colorSlider1);
	colorSlider1->setPosition(600, 80);


	zoomSlider = Slider::create();
	zoomSlider->setup();
	getView()->addSubview(zoomSlider);
	zoomSlider->setPosition(600, 140);
    zoomSlider->setShowProgressActive(true);
    zoomSlider->setSliderPosition(0.4);


	frameSlider = Slider::create();
	frameSlider->setup();
//	getView()->addSubview(frameSlider);
	frameSlider->setPosition(1200, 80);
    frameSlider->setSliderPosition(0);
    frameSlider->setShowProgressActive(true);
    frameSlider->setControlWidth(200);
    frameSlider->setTintColor(Color(0,0,1));


	//    mStartButton = TouchButton::create();
	//    auto text = CACHE()->getTextureByAssetPath("images/kidsparade/like.png");
	//    mStartButton->setImage(text);
	//    getView()->addSubview(mStartButton);
	//    mStartButton->setPosition(350,1350);



}


void TouchUI::setActiveFrame(int index){
            onFrameSlected.emit(index);
               
               for(auto thumb : mThumbs){
                   thumb->setAlpha(0.18);
               }
               mThumbs[index]->setAlpha(1);
               
}


void TouchUI::addThumbs(std::vector<gl::TextureRef> textures){
    for(int i=0; i < textures.size(); i++){
        po::scene::ImageViewRef img = ImageView::create(textures[i]);
        getView()->addSubview(img);
        img->setScale(0.1);
        const float margin = 6;
        img->setPosition(1200 + ((img->getScaledWidth()  + margin) * i), 60);
        mThumbs.push_back(img);
        
        img->getSignal(po::scene::MouseEvent::DOWN_INSIDE).connect([=](po::scene::MouseEvent& event){
            setActiveFrame(i);            
            
        });

    }
}


void TouchUI::updateThumbs(std::vector<gl::TextureRef> textures){
    for(int i=0; i < textures.size(); i++){
        mThumbs[i]->setTexture(textures[i]);
    }

}


void TouchUI::update() {

	ci::Color color = Color(CM_HSV, vec3(colorSlider1->getSliderValue(), 1, 1));

}
