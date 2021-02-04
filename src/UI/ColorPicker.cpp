//
// Created by lab101 on 28/12/2020.
//

#include "ColorPicker.h"
#include "poScene/ShapeView.h"
#include "AssetLoader/AssetsCache.h"
#include "cinder/app/App.h"
#include "cinder/gl/scoped.h"

using namespace po::scene;
using namespace ci;

ColorPickerRef ColorPicker::create() {
    ColorPickerRef ref = std::make_shared<ColorPicker>();
    return ref;
}

ColorPicker::ColorPicker() {

}

void ColorPicker::setup() {


    auto text = CACHE()->getTextureByAssetPath("UI/sliderDot.png");


    mDotBorder = po::scene::ImageView::create(text);
    addSubview(mDotBorder);
    mDotBorder->setAlignment(po::scene::Alignment::CENTER_CENTER);


    mDot = po::scene::ImageView::create(text);
    addSubview(mDot);
    mDot->setAlignment(po::scene::Alignment::CENTER_CENTER);
    mDot->setScale(0.7);


    mFbo = gl::Fbo::create(150, 150);


    // touch events
    getSignal(MouseEvent::DOWN_INSIDE).connect([=](po::scene::MouseEvent &event) {
        mIsPressed = true;

        auto pos = event.getLocalPos();
        setColorPosition(pos);
    });

    getSignal(MouseEvent::DRAG_INSIDE).connect([=](po::scene::MouseEvent &event) {
        if (mIsPressed) {
            auto pos = event.getLocalPos();
            setColorPosition(pos);
        }
    });


    getSignal(MouseEvent::DRAG).connect([=](po::scene::MouseEvent &event) {
        if (mIsPressed) {
            auto pos = event.getLocalPos();
            setColorPosition(pos);
        }
    });
    getSignal(MouseEvent::MOVE).connect([=](po::scene::MouseEvent &event) {

        if (mIsPressed) {
            auto pos = event.getLocalPos();
            setColorPosition(pos);
        }
    });


    getSignal(MouseEvent::UP).connect([=](po::scene::MouseEvent &event) {
        mIsPressed = false;
    });


    mGlsl = gl::GlslProg::create(ci::app::loadAsset("passthrough.vert"), ci::app::loadAsset("color.frag"));
    setSize(mFbo->getSize());

    // add slider
    mBrightnessSlider = Slider::create();
    mBrightnessSlider->setup();
    addSubview(mBrightnessSlider);
    mBrightnessSlider->setPosition(0, 170);
    mBrightnessSlider->setControlWidth(150);
    mBrightnessSlider->setSliderPosition(0.5);

    mBrightnessSlider->mOnValueChanged.connect([=] (float v){
       mRenderFbo = true;
       setColorPosition(mDot->getPosition());

    });


    // update to fill the fbo for the initial color set.
    update();
    setColorPosition(vec2(50, 50));


}

void ColorPicker::setColorPosition(ci::vec2 pos) {

    pos.x = clamp<float>(pos.x, 0, mFbo->getWidth() - 1);
    pos.y = clamp<float>(pos.y, 0, mFbo->getHeight() - 1);

    Area area(pos.x, pos.y, pos.x + 1, pos.y + 1);
    Surface sampleSurface = mFbo->readPixels8u(area);
    mSelectedColor = sampleSurface.getPixel(vec2(0, 0));
    mDotBorder->setPosition(pos);

    mDot->setPosition(pos);
    mDot->setFillColor(mSelectedColor);
}


ColorA ColorPicker::getSelectedColor() {
    return mSelectedColor;
}


void ColorPicker::update() {

    if (mRenderFbo) {
        mRenderFbo = false;
        gl::ScopedFramebuffer fbScp(mFbo);
        gl::ScopedViewport fbVP(mFbo->getSize());
        gl::setMatricesWindow(mFbo->getSize());

        mGlsl->uniform("uBrightness",mBrightnessSlider->getSliderValue());
        auto scoped = gl::ScopedGlslProg(mGlsl);

        gl::color(1, 1, 1, 1);
        ci::Rectf rect = Rectf(0, 0, getWidth(), getHeight());
        gl::color(1, 1, 1);
        gl::drawSolidRect(rect);

        gl::setMatricesWindow(ci::app::getWindowSize());

        if (mHueImage == nullptr) {
            mHueImage = ImageView::create(mFbo->getColorTexture());
            addSubview(mHueImage);
            moveSubviewToBack(mHueImage);
        } else {
            mHueImage->setTexture(mFbo->getColorTexture());
        }
    }

    if (mIsPressed) {
        mDot->setScale(1.2);
        mDotBorder->setScale(1.5);
    } else {
        mDot->setScale(0.7);
        mDotBorder->setScale(1);

    }
}

