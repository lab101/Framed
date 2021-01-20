//
// Created by lab101 on 28/12/2020.
//

#include "Slider.h"
#include "poScene/ShapeView.h"
#include "mathHelper.h"
#include "AssetLoader/AssetsCache.h"

using namespace po::scene;
using namespace ci;

SliderRef Slider::create()
{
    SliderRef ref = std::make_shared<Slider>();
    return ref;
}

Slider::Slider(){

}

void Slider::setup() {

    auto text = CACHE()->getTextureByAssetPath("UI/sliderSegment.png");
    mLineImage = po::scene::ImageView::create(text);
    addSubview(mLineImage);
    mLineImage->setScale(mControlWidth,1);
    mLineImage->setAlignment(po::scene::Alignment::CENTER_LEFT);
    mLineImage->setFillColor(ci::Color(151/255.0,151/255.0,151/255.0));

    mBlackRect = ShapeView::createRect(30,30);
    mBlackRect->setFillColor(ci::Color(0,0,0));
    addSubview(mBlackRect);
    mBlackRect->setAlignment(po::scene::Alignment::CENTER_CENTER);

    text = CACHE()->getTextureByAssetPath("UI/sliderDot.png");
    mDot = po::scene::ImageView::create(text);
    addSubview(mDot);
    mDot->setAlignment(po::scene::Alignment::CENTER_CENTER);


    // touch events
    getSignal(MouseEvent::DOWN_INSIDE).connect([=] (po::scene::MouseEvent& event){
        mIsPressed = true;
        mStartPosition = event.getLocalPos().x;
        mCurrentAbsolutePosition = mStartPosition;
        updateMove(event);
        mDot->setScale(1.2);
    });

    getSignal(MouseEvent::DRAG_INSIDE).connect([=] (po::scene::MouseEvent& event){
        if(mIsPressed)
        {
            updateMove(event);
        }
    });


    getSignal(MouseEvent::DRAG).connect([=] (po::scene::MouseEvent& event){
        if(mIsPressed)
        {
            updateMove(event);
        }
    });
    getSignal(MouseEvent::MOVE).connect([=] (po::scene::MouseEvent& event){

        if(mIsPressed)
        {
            updateMove(event);
        }
    });

    getSignal(MouseEvent::UP_INSIDE).connect([=] (po::scene::MouseEvent& event){
        mIsPressed = false;
        mDot->setScale(1.0);
    });

    getSignal(MouseEvent::UP).connect([=] (po::scene::MouseEvent& event){
       mIsPressed = false;
       mDot->setScale(1.0);
    });

   // setDrawBounds(true);
    setSliderPosition(0.5);


}

void Slider::updateMove(MouseEvent &event) {
    float diff = event.getLocalPos().x - mStartPosition;
    mStartPosition = event.getLocalPos().x;
    mCurrentAbsolutePosition += diff;

    updateFromNewPosition();
    //setSliderPosition(mSliderValue);
}

void Slider::updateFromNewPosition(){
    mSliderValue =  lab101::ofMap(mCurrentAbsolutePosition,0,mLineImage->getScaledWidth(),0.,1,true);
    setSliderPosition(mSliderValue);

    mOnValueChanged.emit(mSliderValue);

    if(mProgressLine){
        mProgressLine->setScale(mSliderValue * mControlWidth,1);
    }

}

void Slider::setSliderPosition(float value) {

    float width = mLineImage->getScaledWidth();
    mDot->setPosition(width * value,0);
    mBlackRect->setPosition(width * value,0);
    mSliderValue = value;
    if(mProgressLine){
        mProgressLine->setScale(mSliderValue * mControlWidth,1);
    }
}

float Slider::getSliderValue() {
    return mSliderValue;
}

void Slider::setTintColor(ci::Color color) {
    if(mDot) mDot->setFillColor(color);
    if(mProgressLine) mProgressLine->setFillColor(color);

}

void Slider::setShowProgressActive(bool value) {
    if(!mProgressLine){
        auto text = CACHE()->getTextureByAssetPath("UI/sliderSegment.png");
        mProgressLine = po::scene::ImageView::create(text);
        addSubview(mProgressLine);
        mProgressLine->setScale(mSliderValue * mControlWidth,1);
        mProgressLine->setAlignment(po::scene::Alignment::CENTER_LEFT);

        moveSubviewToFront(mBlackRect);
        moveSubviewToFront(mDot);
    }
}

void Slider::setControlWidth(float value){
    mControlWidth =  value;
    if(mLineImage)  mLineImage->setScale(mControlWidth,1);
    if(mIconImage)  mIconImage->setPosition(mControlWidth + mIconImage->getScaledWidth(),0);


}

void Slider::setLabel(std::string path){

    auto text = CACHE()->getTextureByAssetPath(path);
    if(mIconImage == nullptr){
        mIconImage = ImageView::create(text);
        addSubview(mIconImage);
        mIconImage->setAlignment(po::scene::Alignment::CENTER_CENTER);
    }else{
        mIconImage->setTexture(text);
    }

    mIconImage->setPosition(mControlWidth + mIconImage->getScaledWidth(),0);


}


