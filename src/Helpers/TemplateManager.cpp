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



void TemplateManager::loadTemplate(ci::fs::path path){
    if( !fs::exists( path ) ){
        return;
    }

    mFrames.clear();
    
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
