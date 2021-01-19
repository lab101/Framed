
#include <stdio.h>
#include "GlobalSettings.h"
#include "cinder/Utilities.h"

using namespace std;
using namespace ci;



void GlobalSettings::setup(std::string appName) {

	mSettingManager.setAppName(appName);
	mSettingManager.readSettings();

	debugMode = Setting<bool>("debug", true);
	mSettingManager.addSetting(&debugMode);

	isMouseOn = Setting<bool>("isMouseOn", true);
	mSettingManager.addSetting(&isMouseOn);

	isFullscreen = Setting<bool>("isFullscreen", false);
	mSettingManager.addSetting(&isFullscreen);


	screenScale = Setting<float>("screenScale", 1.0).setSteps(0.001f);
	screenScale.setMax(2.0f);
	screenScale.setMin(.1f);
	mSettingManager.addSetting(&screenScale);

	backendUrl = SettingString("backendUrl", "http://amazone.lab101.be:4000");
	mSettingManager.addSetting(&backendUrl);

	screenWidth = Setting<int>("screenwidth", 1080);
	mSettingManager.addSetting(&screenWidth);

	screenHeight = Setting<int>("screenHeight", 1080);
	mSettingManager.addSetting(&screenHeight);

	resetScreenTime = Setting<int>("resetScreenTime", 60);
	mSettingManager.addSetting(&resetScreenTime);

	//DATA
	maxItemBuffer = Setting<int>("maxItemBuffer", 100);
	mSettingManager.addSetting(&maxItemBuffer);

	maxRequestQueue = Setting<int>("maxRequestQueue", 1);
	mSettingManager.addSetting(&maxRequestQueue);

	maxSceneItems = Setting<int>("maxSceneItems", 50);
	mSettingManager.addSetting(&maxSceneItems);

	makeBoxSize = Setting<int>("makeBoxSize", 50);
	mSettingManager.addSetting(&makeBoxSize);


	// noise

	flowGravity = Setting<float>("flowGravity", 4);
	mSettingManager.addSetting(&flowGravity);


	flowItemScale = Setting<float>("flowItemScale", 0.1f);
	mSettingManager.addSetting(&flowItemScale);

	flowNoiseScale = Setting<float>("flowNoiseScale", 0.1f);
	mSettingManager.addSetting(&flowNoiseScale);

	flowNoiseSpeed = Setting<float>("flowNoiseSpeed", 0.1f);
	mSettingManager.addSetting(&flowNoiseSpeed);

	flowNoiseSize = Setting<float>("flowNoiseSize", 200);
	mSettingManager.addSetting(&flowNoiseSize);

	flowNoiseAttractionForce = Setting<float>("flowNoiseAttractionForce", 1);
	mSettingManager.addSetting(&flowNoiseAttractionForce);

	flowNoiseAttractionForceLerp = Setting<float>("flowNoiseAttractionForceLerp", 1);
	mSettingManager.addSetting(&flowNoiseAttractionForceLerp);

	// bubble
	repulsionRadius = Setting<float>("repulsionRadius", 400);
	mSettingManager.addSetting(&repulsionRadius);

	maxVelocity = Setting<float>("maxVelocity", 100);
	mSettingManager.addSetting(&maxVelocity);

	repulsionForce = Setting<float>("repulsionForce", 2);
	mSettingManager.addSetting(&repulsionForce);

	test1 = Setting<float>("test1", 0.1f);
	mSettingManager.addSetting(&test1);

	test2 = Setting<float>("test2", 0.1f);
	mSettingManager.addSetting(&test2);

	test3 = Setting<float>("test3", 0.1f);
	mSettingManager.addSetting(&test3);


}


float GlobalSettings::getRadiusScaleByPriority(int& priority) {

	float radiusScale = 1;

	switch (priority)
	{
		// 0  spotlight
	case 0:
		radiusScale = 2;
		break;
		// 1  very high
	case 1:
		radiusScale = 1.8;
		break;
		// 2  high
	case 2:
		radiusScale = 1.6;
		break;
		// 3  medium
	case 3:
		radiusScale = 1.0;
		break;
		// 4  low
	case 4:
		radiusScale = 0.8;
		break;
		// 5  very low
	case 5:
		radiusScale = 0.6;
		break;
	default:
		break;
	}

	return radiusScale;
}


void GlobalSettings::loadSettingSet1() {
	maxSceneItems.setValue(406);
	maxItemBuffer.setValue(318);
	maxRequestQueue.setValue(1);
	flowItemScale.setValue(0.1);

	flowNoiseScale.setValue(-0.399);
	flowNoiseSpeed.setValue(0.112);
	flowNoiseSize.setValue(1127);
	flowNoiseAttractionForce.setValue(-0.24);
	flowNoiseAttractionForceLerp.setValue(8.25);
	makeBoxSize.setValue(210);

	repulsionRadius.setValue(318);
	maxVelocity.setValue(95);
	repulsionForce.setValue(35);
	test1.setValue(6.613);
	test2.setValue(8);
}


GlobalSettings::GlobalSettings() {
}

ci::vec2 GlobalSettings::getScreenSize() {
	return ci::vec2(screenWidth.value(), screenHeight.value());
}


ci::vec2 GlobalSettings::getScreenCenter() {
	return ci::vec2(screenWidth.value() * 0.5f, screenHeight.value() * 0.5f);
}




