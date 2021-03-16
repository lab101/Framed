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
  //  setupCamera();

}

void OverlayManager::setupCamera() {

#if defined( CINDER_MSW_DESKTOP ) || defined( CINDER_OSX_DESKTOP )
    try {
        mCapture = nullptr;
        auto device = deviceList[mSelectedWebcam];
        if (device->checkAvailable()) {
            mCapture = Capture::create(640, 480, device);
            mCapture->start();
            isWebcamStarted = true;
            isLive = true;
        }
    }
    catch (...) {
        isWebcamStarted = false;
    }
#endif
}


void OverlayManager::stopCamera() {
#if defined( CINDER_MSW_DESKTOP ) || defined( CINDER_OSX_DESKTOP )
    isLive = false;
	isWebcamStarted = false;
	try {
		if (mCapture) mCapture->stop();
	}
	catch (...) {

	}
	mCapture = nullptr;
#endif
}

void OverlayManager::setTexture(int index, ci::gl::TextureRef texture){
    mFrames[index] = texture;
}


std::vector<std::string> OverlayManager::getWebcamList()
{
    webcamList.clear();

#if defined( CINDER_MSW_DESKTOP ) || defined( CINDER_OSX_DESKTOP )
    deviceList = Capture::getDevices(true);
    for (const auto& device : deviceList) {
        webcamList.push_back(device->getName());
    }
#endif
    return webcamList;

}


void OverlayManager::snap(){
    
    if(!isWebcamStarted && mCapture != nullptr) return;
    isLive = !isLive;

}

void OverlayManager::update(){

#if defined( CINDER_MSW_DESKTOP ) || defined( CINDER_OSX_DESKTOP )
    if(isWebcamStarted && isLive && mCapture && mCapture->checkNewFrame() ) {
       gl::TextureRef newTexture  = gl::Texture::create( *mCapture->getSurface(), gl::Texture::Format().loadTopDown() );
        
        if(mActiveFrameIndex >= mFrames.size()){
            mFrames[mActiveFrameIndex] = newTexture;
        }
    }
#endif
}



void OverlayManager::clearAll() {
	mFrames.clear();
	mActiveFrameIndex = 0;
}


void OverlayManager::setActiveFrame(int id) {
	mActiveFrameIndex = id;
}

void OverlayManager::nextFrame() {
	if (++mActiveFrameIndex >= mFrames.size()) {
		mActiveFrameIndex = 0;
	}
}

void OverlayManager::prevFrame() {
	if (--mActiveFrameIndex < 0) {
		mActiveFrameIndex = mFrames.size() - 1;
	}
}



int OverlayManager::getActiveFrame() {
	return mActiveFrameIndex;
}


void OverlayManager::drawAtIndex(int index) {

	if (mFrames.size() == 0) return;

	//int rangeIndex = lab101::getInRangeIndex(index, mFrames.size());

	//if (rangeIndex < 0 || rangeIndex > mFrames.size()) return;

	if(mFrames.find(index) == mFrames.end()) return;

	
	ci::gl::TextureRef texture = mFrames[index];
	if (texture != nullptr) {

		// ceci::gnter the overlay.
		float height = mSize.y;
		float width = texture->getWidth() * mSize.y / texture->getHeight();

		float offsetCenter = (mSize.x - width) * 0.5;

		ci::Rectf frame = ci::Rectf(offsetCenter, 0, offsetCenter + width, height);

		//		if (GS()->mirrorWebcam.value() && mFrames[rangeIndex].flipHorizontal) {
			//		frame = ci::Rectf(offsetCenter + width, 0, offsetCenter, height);
				//}

		gl::draw(texture, frame);
	}
}



void OverlayManager::drawGUI() {

	if (ImGui::Combo("available webcams", &(mSelectedWebcam), webcamList)) {
	}

	if (ImGui::Button("reload cameralist",ImVec2(200, 40))) {
		getWebcamList();
	}

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(0, 182, 80 + (sin(app::getElapsedSeconds() * 5) * 60))));
	if (ImGui::Button("start webcam", ImVec2(200, 40))) {
		setupCamera();
	}
	ImGui::PopStyleColor();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(233, 3, 80)));

	if (ImGui::Button("stop webcam", ImVec2(200, 40))) {
		stopCamera();
	}
	ImGui::PopStyleColor();


	ImGui::Checkbox("mirror webcam", &(GS()->mirrorWebcam.value()));
}
