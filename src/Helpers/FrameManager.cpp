//
// Created by lab101 on 21/01/2021.
//

#include "FrameManager.h"
#include "cinder/app/App.h"
#include "cinder/CinderImGui.h"
#include "mathHelper.h"
#include "Lab101Utils.h"


void FrameManager::setup(int nrOfFrames, ci::vec2 size) {

	mSize = size;

	for (int i = 0; i < nrOfFrames; i++) {

		Frame newFrame;
		newFrame.setup(size);
		mFrames.push_back(newFrame);
	}

    mActiveFrameIndex = 0;
    mFrameSpeed = 0.5;
}


ci::ivec2 FrameManager::getSize() {
	return mSize;
}

void FrameManager::clearAll(){
    for(auto f : mFrames){
        f.clearFbo();
    }
}

std::vector<ci::gl::TextureRef> FrameManager::getTextures(){
    std::vector<ci::gl::TextureRef>  textures;
    for(auto f : mFrames){
      //  f.clearFbo();
        auto text = f.getTexture();
        textures.push_back(text);
    }
    
    return  textures;
}

void FrameManager::setActiveFrame(int id){
    mActiveFrameIndex = id;
}

void FrameManager::saveAll(){
    
    std::string mOutputFolder = ci::getDocumentsDirectory().string() + "framed/" + getDateString();
    
    if(ci::fs::exists(mOutputFolder)){
        ci::fs::create_directory(mOutputFolder);
    }

    
    int fileNr = 1;
    for(auto f : mFrames){
        
        std::string mFileName =  mOutputFolder + "/image" + std::to_string(fileNr) + ".png";
        f.writeBuffer(mFileName);
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

int FrameManager::getActiveFrame(){
    return mActiveFrameIndex;
}


void FrameManager::setFrameIndexNormalised(float value) {

	//mActiveFrameIndex = lab101::ofMap(value, 0, 1, 0, mFrames.size() - 1);
}


void FrameManager::draw() {
	mFrames[mActiveFrameIndex].draw();
}

void FrameManager::drawAtIndex(int index) {
    
    index =  mActiveFrameIndex - 1;
    index = index % (mFrames.size() - 1);
    mFrames[index].draw();
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
	ImGui::SliderFloat("speed: ", &mFrameSpeed, 2.f, 20.0f);

	ImGui::End();

}
