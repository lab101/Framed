//
// Created by lab101 on 21/01/2021.
//

#include "FrameManager.h"
#include "cinder/app/App.h"
#include "cinder/CinderImGui.h"
#include "mathHelper.h"

void FrameManager::setup(int nrOfFrames, ci::vec2 size) {

	mSize = size;

	for (int i = 0; i < nrOfFrames; i++) {

		Frame newFrame;
		newFrame.setup(size);
		mFrames.push_back(newFrame);
	}

	mActiveFrameIndex = nrOfFrames - 1;
}


ci::ivec2 FrameManager::getSize() {
	return mSize;
}


void FrameManager::setFrameIndexNormalised(float value) {

	mActiveFrameIndex = lab101::ofMap(value, 0, 1, 0, mFrames.size() - 1);
}


void FrameManager::draw() {
	mFrames[mActiveFrameIndex].draw();
}

void FrameManager::drawLoop() {
	int index = (int)(ci::app::getElapsedSeconds() * mFrameSpeed) % (int)mFrames.size();
	auto text = mFrames[index].getTexture();
	ci::gl::draw(text, ci::Rectf(10, 10, 1600 * 0.25, 1200 * 0.25));
}

void FrameManager::drawPoints(std::vector<ci::vec3>& points, ci::Color color) {
	mFrames[mActiveFrameIndex].drawPoints(points, color);
}

void FrameManager::drawGUI() {
	ImGui::Begin("FrameManager");
	ImGui::SliderInt("index: ", &mActiveFrameIndex, 0, mFrames.size() - 1);
	ImGui::SliderFloat("speed: ", &mFrameSpeed, 0.f, 10.0f);

	ImGui::End();

}