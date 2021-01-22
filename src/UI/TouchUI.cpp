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

ci::Color TouchUI::getColor() {

    float hue = colorSlider1->getSliderValue();
    auto color = Color(CM_HSV, vec3(hue, 0.8, 0.8));
    return color;
}



void TouchUI::setup() {


    colorSlider1 = ColorSlider::create();
    colorSlider1->setup();
    getView()->addSubview(colorSlider1);
    colorSlider1->setPosition(200,80);


    zoomSlider = Slider::create();
    zoomSlider->setup();
    getView()->addSubview(zoomSlider);
    zoomSlider->setPosition(200,140);


//    mStartButton = TouchButton::create();
//    auto text = CACHE()->getTextureByAssetPath("images/kidsparade/like.png");
//    mStartButton->setImage(text);
//    getView()->addSubview(mStartButton);
//    mStartButton->setPosition(350,1350);



}


void TouchUI::update() {

    ci::Color color = Color(CM_HSV, vec3(colorSlider1->getSliderValue(), 1, 1));

}
