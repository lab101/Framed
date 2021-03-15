//
// Created by lab101 on 21/01/2021.
//

#include "TemplateManager.h"

#include "cinder/app/App.h"
#include "cinder/CinderImGui.h"

#include "Lab101Utils.h"
#include "GlobalSettings.h"
#include "mathHelper.h"
#include "FileHelper.h"


using namespace ci;
using namespace ci::app;

void TemplateManager::setup() {

    auto mTemplatePath = app::getAssetPath("").string() + "/template";
    loadTemplate(mTemplatePath);
}


//void TemplateManager::loadOverlayFolders(){
//        auto mOverlayFoldersPath = app::getAssetPath("").string() + "/overlays";
//
//       if( !fs::exists( mOverlayFoldersPath ) ){
//           fs::create_directory(mOverlayFoldersPath);
//       }
//
//        mOverlayFolders.clear();
//        for(fs::directory_iterator it(mOverlayFoldersPath); it != fs::directory_iterator(); ++it ){
//           {
//               if(is_directory( *it )){
//                   mOverlayFolders.push_back(it->path());
//                }
//           }
//       }
//}

void TemplateManager::loadTemplate(ci::fs::path path){
    if( !fs::exists( path ) ){
        return;
    }

    mFrames.clear();
//    int index=0;
    
    std::vector<std::string> extensions = {".png",".jpg",".jpeg"};
    auto files = readDirectory(path.string(),extensions);
    for(auto& file : files){
        try{
            auto text =  gl::Texture::create(loadImage(file));
            mFrames.push_back(text);
        }catch(...){
            std::cout << "error loading " << file << std::endl;
        }
    }
}

void TemplateManager::update(){
     
}




void TemplateManager::drawAtIndex(int index) {
    
    //if(mFrames.find(index) == mFrames.end()) return;
    
//    if(mFrames.size() == 0) return;
//
//    int rangeIndex = lab101::getInRangeIndex(index, mFrames.size());
//
//    if(rangeIndex < 0 || rangeIndex > mFrames.size()) return;
    
    auto texture = mFrames[index];
       if(texture != nullptr){
           
           // center the overlay.
           float height  = mSize.y;
           float width =  texture->getWidth() * mSize.y / texture->getHeight();
        
           float offsetCenter = (mSize.x - width) * 0.5;
           ci::Rectf frame(offsetCenter,0,offsetCenter + width,height);
           
           gl::draw(texture,frame);
       }
}

std::vector<ci::gl::TextureRef> TemplateManager::getTextures(){
    return mFrames;
}


void TemplateManager::drawGUI() {
    
 
}
