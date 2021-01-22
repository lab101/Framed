//
// Created by lab101 on 28/12/2020.
//

#pragma once

#include "poScene/ViewController.h"
#include "ColorSlider.h"
#include "TouchButton.h"


class TouchUI;
typedef std::shared_ptr<TouchUI> TouchUIRef;

class TouchUI : public po::scene::ViewController {

private:

    ColorSliderRef colorSlider1;
    SliderRef zoomSlider;

    TouchButtonRef mStartButton;

    float mScale;

public:
    void setup();
    void update();

    static TouchUIRef create();

    ci::Color getColor();
    float getScale();

};

