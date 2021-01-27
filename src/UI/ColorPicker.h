//
// Created by lab101 on 28/12/2020.
//

#pragma once

#include <poScene/ImageView.h>
#include "poScene/View.h"
#include "cinder/Signals.h"
#include "cinder/gl/gl.h"

typedef std::shared_ptr<class ColorPicker> ColorPickerRef;

class ColorPicker : public po::scene::View
{

    float mStartPosition;
    float mCurrentAbsolutePosition;
    ci::gl::GlslProgRef mGlsl;
    ci::gl::FboRef  mFbo;
    ci::ColorA mSelectedColor;

    bool mRenderFbo = true;
protected:

    float mColorPickerValue;
    bool mIsPressed = false;
    po::scene::ImageViewRef mHueImage = nullptr;
    po::scene::ImageViewRef mDot = nullptr;
    po::scene::ImageViewRef mDotBorder = nullptr;

public:

    ci::signals::Signal<void(float value)> mOnValueChanged;
    static ColorPickerRef create();
    ColorPicker();
    virtual void setup();

    ci::ColorA getSelectedColor();
    
    void draw() override;
    void update() override;


    void setColorPosition(ci::vec2 position);
};

