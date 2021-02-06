//
// Created by lab101 on 21/01/2021.
//

#include "OverlayManager.h"

#include "cinder/app/App.h"
#include "cinder/CinderImGui.h"

#include "Lab101Utils.h"
#include "GlobalSettings.h"
#include "mathHelper.h"

using namespace ci;
using namespace ci::app;

void OverlayManager::setup(int nrOfFrames, ci::vec2 size) {

	mSize = size;
	mActiveFrameIndex = 0;
    
    for(int i =0 ; i < nrOfFrames; ++i){
        gl::TextureRef text = gl::Texture::create(size.x,size.y);
        mFrames.push_back(text);
    }
    
    setupCamera();
}

void OverlayManager::setupCamera(){
    mCapture = Capture::create( 640, 480 );
    mCapture->start();
}

void OverlayManager::snap(){
    
    
    if( mCapture && mCapture->checkNewFrame() ) {

    
    //gl::TextureRef newTexture = gl::Texture::create(*mCapture->getSurface());
        gl::TextureRef newTexture  = gl::Texture::create( *mCapture->getSurface(), gl::Texture::Format().loadTopDown() );

   // auto texture = mFrames[mActiveFrameIndex];
    
    mFrames[mActiveFrameIndex] = newTexture;

    }
//    if(texture != nullptr){
//
//        texture->update(  );
//    }

}

ci::ivec2 OverlayManager::getSize() {
	return mSize;
}

void OverlayManager::clearAll() {
    mFrames.clear();
    mActiveFrameIndex = 0;
    
}


void OverlayManager::setActiveFrame(int id) {
	mActiveFrameIndex = id;
}

void OverlayManager::nextFrame() {
    if(++mActiveFrameIndex >= mFrames.size()){
        mActiveFrameIndex = 0;
    }
}

void OverlayManager::prevFrame() {
    if(--mActiveFrameIndex < 0){
        mActiveFrameIndex = mFrames.size()-1;
    }
}



int OverlayManager::getActiveFrame() {
	return mActiveFrameIndex;
}


void OverlayManager::drawAtIndex(int index) {
    
    int rangeIndex = lab101::getInRangeIndex(index, mFrames.size());
    
    auto texture = mFrames[rangeIndex];
       if(texture != nullptr){
           
           
           float height  = mSize.y;
           float width =  texture->getWidth() * mSize.y / texture->getHeight();
        
           float offsetCenter = (mSize.x - width) * 0.5;
           ci::Rectf frame(offsetCenter,0,offsetCenter + width,height);
           
           gl::draw(texture,frame);
       }
  //	mFrames[rangeIndex].draw();
}


void OverlayManager::drawGUI() {
	ImGui::Begin("OverlayManager");
	ImGui::SliderFloat("speed: ", &GS()->frameSpeed.value(), 2.f, 20.0f);

	ImGui::End();

}
