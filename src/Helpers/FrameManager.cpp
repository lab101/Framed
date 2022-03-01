//
// Created by lab101 on 21/01/2021.
//

#include "FrameManager.h"
#include "cinder/app/App.h"
#include "cinder/CinderImGui.h"
#include "Lab101Utils.h"
#include "GlobalSettings.h"
#include "mathHelper.h"

using namespace ci;
using namespace ci::app;

void FrameManager::setup(int nrOfFrames, ci::vec2 size) {

	mSize = size;

	mFrames.clear();
	changeNrOfFrames(nrOfFrames);

	mActiveFrameIndex = 0;
}


void FrameManager::changeNrOfFrames(int nrOfFrames) {
//	mFrames.clear();

	// erase frames
	if (nrOfFrames < mFrames.size()) {

		mFrames.erase(mFrames.begin() + nrOfFrames, mFrames.begin() + mFrames.size());
	}else{

		// add new frames
		int newFramesCount = nrOfFrames - mFrames.size();
		for (int i = 0; i < newFramesCount; i++) {
			Frame* newFrame = new Frame();;
			newFrame->setup(mSize);
			mFrames.push_back(newFrame);
		}
	}



}


ci::ivec2 FrameManager::getSize() {
	return mSize;
}

void FrameManager::clearAll() {
	for (auto& f : mFrames) {
		f->clearFbo();
	}
	mActiveFrameIndex = 0;
}

void FrameManager::drawTextures(std::vector<gl::TextureRef> textures) {
	if (textures.size() == 0) return;

	int textureIndex = 0;
	for (auto& f : mFrames) {
		f->drawTexture(textures[textureIndex]);
		if (++textureIndex >= textures.size()) return;
	}
}

std::vector<ci::gl::TextureRef> FrameManager::getTextures() {
	std::vector<ci::gl::TextureRef>  textures;
	for (auto& f : mFrames) {
		auto text = f->getTexture();
		if (text) textures.push_back(text);
	}

	return  textures;
}

void FrameManager::setActiveFrame(int id) {
	mActiveFrameIndex = id;
}

void FrameManager::nextFrame() {
	if (++mActiveFrameIndex >= mFrames.size()) {
		mActiveFrameIndex = 0;
	}
}

void FrameManager::prevFrame() {
	if (--mActiveFrameIndex < 0) {
		mActiveFrameIndex = mFrames.size() - 1;
	}
}

void FrameManager::saveAll() {

	std::string mOutputFolder = ci::getDocumentsDirectory().string() + "framed/" + getDateString();

	if (ci::fs::exists(mOutputFolder)) {
		ci::fs::create_directory(mOutputFolder);
	}


	int fileNr = 1;
	for (auto f : mFrames) {

		std::string mFileName = mOutputFolder + "/image" + std::to_string(fileNr) + ".png";
		f->writeBuffer(mFileName);
		fileNr++;
		// write interpolated points to a data file in the output folder
//        std::string dataFilePath = mOutputFolder + "/data.txt";
//        try{
//            std::ofstream dataFile;
//
//            dataFile.open(dataFilePath);
//
//            for(pointVec s :  strokes){
//                for(vec3& p :  s){
//                    dataFile << p.x << "," << p.y << "," << p.z << ";";
//                }
//                dataFile <<  std::endl;
//            }
//
//            dataFile.close();
//        }catch(...){
//            CI_LOG_E( "couldn't write to path: " + dataFilePath);
//        }
	}

}

int FrameManager::totalFrameCount() {
	return mFrames.size();
}

int FrameManager::getActiveFrame() {
	return mActiveFrameIndex;
}

ci::gl::TextureRef FrameManager::getActiveTexture() {
	return mFrames[mActiveFrameIndex]->getTexture();
}


void FrameManager::draw(bool isFullscreen) {

	if (isFullscreen) {
		auto text = mFrames[mActiveFrameIndex]->getTexture();

		if (text) {
			ci::gl::draw(text, ci::Rectf(0, 0, app::getWindowWidth(), app::getWindowHeight()));
		}
	}
	else {
		mFrames[mActiveFrameIndex]->draw();
	}

}

void FrameManager::drawAtIndex(int index) {

	int rangeIndex = lab101::getInRangeIndex(mActiveFrameIndex + index, mFrames.size());
	mFrames[rangeIndex]->draw();
}

ci::gl::TextureRef FrameManager::getLoopTexture() {
	const float frameSpeed = GS()->frameSpeed.value();
	int index = (int)(ci::app::getElapsedSeconds() * frameSpeed) % (int)mFrames.size();
	return  mFrames[index]->getTexture();
}


void FrameManager::drawLoop(bool isFullscreen) {

	auto text = getLoopTexture();
	auto rect = ci::Rectf(10, 10, 400, mSize.y * 400 / mSize.x);
	if (isFullscreen) rect.set(0, 0, app::getWindowWidth(), app::getWindowHeight());
	ci::gl::draw(text, rect);
}

void FrameManager::drawPoints(std::vector<ci::vec3>& points, ci::Color color, int frameId) {
	if (frameId == -1) frameId = mActiveFrameIndex;
	if (frameId < mFrames.size()) mFrames[frameId]->drawPoints(points, color);
}

void FrameManager::drawCircle(ci::vec2 p1, ci::vec2 p2, ci::Color color, int frameId) {
	if (frameId == -1) frameId = mActiveFrameIndex;
	if (frameId < mFrames.size()) mFrames[frameId]->drawCircle(p1, p2, color);
}

void FrameManager::drawRectangle(ci::vec2 p1, ci::vec2 p2, ci::Color color, int frameId) {
	if (frameId == -1) frameId = mActiveFrameIndex;
	if (frameId < mFrames.size()) mFrames[frameId]->drawRectangle(p1, p2, color);
}

void FrameManager::drawGUI() {
	//ImGui::Begin("FrameManager");
	//ImGui::End();

}
