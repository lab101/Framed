//
// Created by lab101 on 07/01/2021.
//

#pragma once

#include "Slider.h"

typedef std::shared_ptr<class ColorSlider> ColorSliderRef;

class ColorSlider : public Slider {
    float mHue;
public:
    static ColorSliderRef create();

    void update() override;
    void setup()  override;


};

