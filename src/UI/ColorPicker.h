//
// Created by lab101 on 28/12/2020.
//

#pragma once

#include <poScene/ImageView.h>
#include "poScene/View.h"
#include "cinder/Signals.h"
#include "cinder/gl/gl.h"
#include "Slider.h"

typedef std::shared_ptr<class ColorPicker> ColorPickerRef;

class ColorPicker : public po::scene::View
{
    ci::gl::GlslProgRef mGlsl;
    ci::gl::FboRef  mFbo;
    ci::ColorA mSelectedColor;
    bool mIsPressed = false;
    bool mRenderFbo = true;
    
    std::vector<po::scene::ImageViewRef> mHistory;

protected:
    po::scene::ImageViewRef mHueImage = nullptr;
    po::scene::ImageViewRef mDot = nullptr;
    po::scene::ImageViewRef mDotBorder = nullptr;
    SliderRef mBrightnessSlider;

public:

    static ColorPickerRef create();
    ColorPicker();

    void setColorPosition(ci::vec2 position);
    ci::ColorA getSelectedColor();
    ci::signals::Signal<void(float value)> mOnValueChanged;

    void setup() override;
    void update() override;

};

