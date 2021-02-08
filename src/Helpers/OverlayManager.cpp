//
// Created by lab101 on 21/01/2021.
//

#include "OverlayManager.h"

#include "cinder/app/App.h"
#include "cinder/CinderImGui.h"

#include "Lab101Utils.h"
#include "GlobalSettings.h"
#include "mathHelper.h"
#include "FileHelper.h"


using namespace ci;
using namespace ci::app;

void OverlayManager::setup(int nrOfFrames, ci::vec2 size) {

	mSize = size;
	mActiveFrameIndex = 0;
    
//    for(int i =0 ; i < nrOfFrames; ++i){
//        gl::TextureRef text = gl::Texture::create(size.x,size.y);
//        mFrames.push_back(text);
//    }
    
    setupCamera();
    loadOverlayFolders();
}

void OverlayManager::setupCamera(){
    if(GS()->hasWebcam.value() && !isWebcamStarted){
        try{
            mCapture = Capture::create( 640, 480 );
            mCapture->start();
            isWebcamStarted = true;
            isLive = true;
        }catch(...){
            isWebcamStarted = false;
        }
    }
}

void OverlayManager::snap(){
    
    if(!isWebcamStarted && mCapture != nullptr) return;
    
    isLive = !isLive;
}

void OverlayManager::loadOverlayFolders(){
        auto mOverlayFoldersPath = app::getAssetPath("").string() + "/overlays";

       if( !fs::exists( mOverlayFoldersPath ) ){
           fs::create_directory(mOverlayFoldersPath);
       }

        mOverlayFolders.clear();
        for(fs::directory_iterator it(mOverlayFoldersPath); it != fs::directory_iterator(); ++it ){
           {
               if(is_directory( *it )){
                   mOverlayFolders.push_back(it->path());
                }
           }
       }
}

void OverlayManager::loadOverlay(ci::fs::path path){
    if( !fs::exists( path ) ){
        return;
    }
    
    mFrames.clear();
    int index=0;
    for(fs::directory_iterator it(path); it != fs::directory_iterator(); ++it )
    {
            std::string extension = it->path().extension().string();
            if(extension == ".jpg" || extension == ".png"){
                try{
                    auto text =  gl::Texture::create(loadImage(it->path()));
                    mFrames.push_back(text);
                    
                    // stop when more overlays than frames;
                    if(++index > mFrames.size()) return;
                }catch(...){
                    std::cout << "error loading " <<  it->path().string() << std::endl;
                }
            }
    }
}

void OverlayManager::update(){
    
    
    if(isWebcamStarted && isLive && mCapture && mCapture->checkNewFrame() ) {
       gl::TextureRef newTexture  = gl::Texture::create( *mCapture->getSurface(), gl::Texture::Format().loadTopDown() );
        
        if(mActiveFrameIndex >= mFrames.size()){
            mFrames.push_back(newTexture);
        }else{
            mFrames[mActiveFrameIndex] = newTexture;
        }
   }
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
    if(mFrames.size() == 0) return;
    
    int rangeIndex = lab101::getInRangeIndex(index, mFrames.size());
    
    if(rangeIndex < 0 || rangeIndex > mFrames.size()) return;
    
    auto texture = mFrames[rangeIndex];
       if(texture != nullptr){
           
           // center the overlay.
           float height  = mSize.y;
           float width =  texture->getWidth() * mSize.y / texture->getHeight();
        
           float offsetCenter = (mSize.x - width) * 0.5;
           ci::Rectf frame(offsetCenter,0,offsetCenter + width,height);
           
           gl::draw(texture,frame);
       }
}


void OverlayManager::drawGUI() {
    
    if (ImGui::Combo("overlays", &(selectedOverlayFolder), getStringList(mOverlayFolders))) {
            loadOverlay(mOverlayFolders[selectedOverlayFolder]);
        }
}
