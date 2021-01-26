//
// Created by lab101 on 28/12/2020.
//

#pragma once

#include <poScene/ImageView.h>
#include "poScene/View.h"
#include "cinder/Signals.h"

typedef std::shared_ptr<class Slider> SliderRef;

class Slider : public po::scene::View
{

    float mStartPosition;
    float mCurrentAbsolutePosition;
    void updateFromNewPosition();


protected:

    float mSliderValue;
    bool mIsPressed = false;
    po::scene::ImageViewRef mLineImage = nullptr;
    po::scene::ImageViewRef mIconImage = nullptr;
    po::scene::ImageViewRef mProgressLine = nullptr;

    po::scene::ShapeViewRef mBlackRect;
    po::scene::ImageViewRef mDot = nullptr;

    float mControlWidth = 360;

public:

    ci::signals::Signal<void(float value)> mOnValueChanged;
    static SliderRef create();
    Slider();
    virtual void setup();
    virtual float getSliderValue();

    void updateMove(po::scene::MouseEvent &event);
    void setSliderPosition(float pct);
    void setTintColor(ci::Color color);
    void setShowProgressActive(bool value);
    void setControlWidth(float value);

    void setLabel(std::string path);


    };
