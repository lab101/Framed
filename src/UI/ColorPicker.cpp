//
// Created by lab101 on 28/12/2020.
//

#include "ColorPicker.h"
#include "poScene/ShapeView.h"
#include "mathHelper.h"
#include "AssetLoader/AssetsCache.h"
#include "cinder/App/App.h"
#include "cinder/gl/Scoped.h"

using namespace po::scene;
using namespace ci;

ColorPickerRef ColorPicker::create()
{
    ColorPickerRef ref = std::make_shared<ColorPicker>();
    return ref;
}

ColorPicker::ColorPicker(){

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
    

    mFbo = gl::Fbo::create(150,150);
    
    //setDrawBounds(true);


    // touch events
    getSignal(MouseEvent::DOWN_INSIDE).connect([=] (po::scene::MouseEvent& event){
        mIsPressed = true;
  
        auto pos =  event.getLocalPos();
        Area area(pos.x,pos.y,pos.x+1,pos.y+1);
        Surface mySurface = mFbo->readPixels8u(area);
        mSelectedColor = mySurface.getPixel(vec2(0,0));

        
        mDotBorder->setPosition(pos);

        mDot->setPosition(pos);
        mDot->setFillColor(mSelectedColor);

    });

    getSignal(MouseEvent::DRAG_INSIDE).connect([=] (po::scene::MouseEvent& event){
        if(mIsPressed)
        {
        }
    });


    getSignal(MouseEvent::DRAG).connect([=] (po::scene::MouseEvent& event){
        if(mIsPressed)
        {
        }
    });
    getSignal(MouseEvent::MOVE).connect([=] (po::scene::MouseEvent& event){

        if(mIsPressed)
        {
        }
    });

    
    
     mGlsl = gl::GlslProg::create( ci::app::loadAsset( "passthrough.vert" ), ci::app::loadAsset( "color.frag" ) );

    setSize(mFbo->getSize());


}


ColorA ColorPicker::getSelectedColor(){
    return mSelectedColor;
}




void ColorPicker::update(){
    {
           
           gl::ScopedFramebuffer fbScp( mFbo );
           gl::ScopedViewport fbVP (mFbo->getSize());
           gl::setMatricesWindow( mFbo->getSize() );
           auto scoped  = gl::ScopedGlslProg(mGlsl);

           gl::color(1, 1, 1, 1);
           ci::Rectf rect =  Rectf(0,0,getWidth(),getHeight());
           gl::color(1,1,1);
           gl::drawSolidRect(rect);

           gl::setMatricesWindow( ci::app::getWindowSize() );

           
        if(mHueImage == nullptr){
            mHueImage = ImageView::create(mFbo->getColorTexture());
            addSubview(mHueImage);
            moveSubviewToBack(mHueImage);
        }else{
            mHueImage->setTexture(mFbo->getColorTexture());
        }
        
       }
}


void ColorPicker::draw(){
    
    View::draw();

//    gl::draw(mFbo->getColorTexture());

    


}
