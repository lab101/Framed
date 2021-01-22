//
// Created by lab101 on 07/01/2021.
//

#include "ColorSlider.h"
#include "poScene/ShapeView.h"
#include "mathHelper.h"
#include "AssetLoader\AssetsCache.h"

using namespace po::scene;
using namespace ci;

ColorSliderRef ColorSlider::create()
{
	ColorSliderRef ref = std::make_shared<ColorSlider>();
	return ref;
}


void ColorSlider::update() {
	po::scene::View::update();
	mHue = lab101::ofMap(mSliderValue, 0, 1, 0.1, 1, true);
	auto color = Color(CM_HSV, vec3(mHue, 1, 1));
	mDot->setFillColor(color);
}

void ColorSlider::setup() {
	Slider::setup();

	auto text = CACHE()->getTextureByAssetPath("UI/colorslider.png");
	mLineImage->setTexture(text);
	mLineImage->setScale(1, 1);

}
