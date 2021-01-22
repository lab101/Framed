//
// Created by lab101 on 21/01/2021.
//

#include "Frame.h"
#include "GlobalSettings.h"
#include "BrushManager.hpp"

using namespace  ci;

void Frame::setup(ci::vec2 size){
    setFbo(mActiveFbo,size, 1);
}


void Frame::setFbo(ci::gl::FboRef& fbo,ci::ivec2 size,float windowScale){

    gl::Fbo::Format format;
    format.setColorTextureFormat( gl::Texture2d::Format().internalFormat( GL_RGBA32F ) );

    gl::enableAlphaBlending();
    // format.setSamples( 4 );
    fbo = gl::Fbo::create(size.x, size.y ,format );

    clearFbo();
}

void Frame::clearFbo(){
    // clear the screen;
    if(!mActiveFbo) return;

    gl::ScopedFramebuffer fbScp( mActiveFbo );
    gl::clear( GS()->fboBackground );
}

void Frame::writeBuffer(std::string path){
    auto source = mActiveFbo->getColorTexture()->createSource();
    
    std::thread threadObj([=]{
           
           try{
               writeImage(path, source);
           }catch(...){
               CI_LOG_E("error writing PNG file: " + path);
           }
           
       });
       
       threadObj.detach();

}

void Frame::draw(){
    gl::draw(mActiveFbo->getColorTexture());
}

ci::gl::Texture2dRef Frame::getTexture(){
    return mActiveFbo->getColorTexture();
}


void Frame::drawPoints(std::vector<ci::vec3>& points,ci::Color color){


    if(points.size() > 0){

        gl::ScopedFramebuffer fbScp( mActiveFbo );
        gl::ScopedViewport fbVP (mActiveFbo->getSize());
        gl::setMatricesWindow( mActiveFbo->getSize() );
        gl::ScopedBlendPremult scpBlend;

        gl::color(1, 1, 1, 1);

        BrushManagerSingleton::Instance()->drawBrush(points, 0.98,color);
        gl::setMatricesWindow( ci::app::getWindowSize() );
    }
}



