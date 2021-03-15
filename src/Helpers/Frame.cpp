//
// Created by lab101 on 21/01/2021.
//

#include "Frame.h"
#include "GlobalSettings.h"
#include "BrushManager.h"

using namespace  ci;

void Frame::setup(ci::vec2 size) {
	setFbo(mActiveFbo, size, 1);
}


void Frame::setFbo(ci::gl::FboRef& fbo, ci::ivec2 size, float windowScale) {

	gl::Fbo::Format format;
	//   format.setColorTextureFormat( gl::Texture2d::Format().internalFormat( GL_RGBA32F ) );
	format.setSamples(0);
	format.disableDepth();
	mActiveFbo = gl::Fbo::create(size.x, size.y, format);
	clearFbo();
}

void Frame::clearFbo() {
	if (!mActiveFbo) return;
	gl::ScopedFramebuffer fbScp(mActiveFbo);
	gl::clear(GS()->fboBackground);
}

void Frame::drawTexture(ci::gl::TextureRef text){
    if(!mActiveFbo) return;
    gl::ScopedFramebuffer fbScp( mActiveFbo );
    
    gl::ScopedViewport fbVP (mActiveFbo->getSize());
    gl::setMatricesWindow( mActiveFbo->getSize() );
    gl::ScopedBlendPremult scpBlend;
    
    ci::Rectf rec(0,0,mActiveFbo->getSize().x,mActiveFbo->getSize().y);
    gl::draw(text,rec);

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

void Frame::draw() {
	if (mActiveFbo)  gl::draw(mActiveFbo->getColorTexture());
}

ci::gl::Texture2dRef Frame::getTexture() {
	if (mActiveFbo) return mActiveFbo->getColorTexture();
	return nullptr;
}

void Frame::activateFbo() {

}

void Frame::drawPoints(std::vector<ci::vec3>& points, ci::Color color) {

	if (points.size() > 0 && mActiveFbo != nullptr) {

		gl::ScopedFramebuffer fbScp(mActiveFbo);
		gl::ScopedViewport fbVP(mActiveFbo->getSize());
		gl::setMatricesWindow(mActiveFbo->getSize());
		gl::ScopedBlendPremult scpBlend;
	

		gl::color(1, 1, 1, 1);

		BrushManagerSingleton::Instance()->drawBrush(points, 0.98, color);
		gl::setMatricesWindow(ci::app::getWindowSize());
	}
}


void Frame::drawCircle(ci::vec2 point1, ci::vec2 point2, ci::Color color) {

	gl::ScopedFramebuffer fbScp(mActiveFbo);
	gl::ScopedViewport fbVP(mActiveFbo->getSize());
	gl::setMatricesWindow(mActiveFbo->getSize());
	gl::ScopedBlendPremult scpBlend;

	auto stock  = ci::gl::getStockShader(gl::ShaderDef().color());
	gl::ScopedGlslProg scpd(stock);

	gl::color(color);
	float radius = fabsf(glm::distance(point1, point2));
	//gl::color(1, 1, 1, 0);

	ci::app::console() << point1 << " - " << radius << std::endl;
	gl::drawSolidCircle(point1,radius);
	
	gl::setMatricesWindow(ci::app::getWindowSize());


}


