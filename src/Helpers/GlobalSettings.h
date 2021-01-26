#pragma once

#include "cinder/app/App.h"


#include "Singleton.h"
#include "Settings/SettingManager.h"
#include "Settings/SettingString.h"

class GlobalSettings {


public:

	// General
	SettingManager mSettingManager;

	Setting<bool>   debugMode;
	Setting<bool>   isFullscreen;
    Setting<bool>   isMouseOn;
    Setting<bool>   projectorMode;

	GlobalSettings();

	Setting<int> screenWidth;
	Setting<int> screenHeight;
	Setting<int> resetScreenTime;
	Setting<float> screenScale;


    Setting<float> performanceDownScale;
    Setting<int> maxGroups;


    void setup(std::string appName);

	ci::vec2 getScreenSize();
	ci::vec2 getScreenCenter();

	// Data
	Setting<int> maxItemBuffer;
	Setting<int> maxRequestQueue;
	Setting<int> maxSceneItems;
	Setting<int> makeBoxSize;

	// noise


	Setting<float>  flowGravity;
	Setting<float>  flowItemScale;
	Setting<float>  flowNoiseScale;
	Setting<float>  flowNoiseSpeed;
	Setting<float>  flowNoiseSize;
	Setting<float>  flowNoiseAttractionForce;
	Setting<float>  flowNoiseAttractionForceLerp;

	// bubble
	Setting<float>  repulsionRadius;
	Setting<float>  repulsionForce;
	Setting<float>  maxVelocity;
	Setting<float>  test1;
	Setting<float>  test2;
	Setting<float>  zoomLevel;



    ci::ColorA fboBackground;

	void loadSettingSet1();
	float getRadiusScaleByPriority(int& priority);

	SettingString backendUrl;


};



typedef Singleton<GlobalSettings> GlobalSettingsSingleton;

inline GlobalSettings* GS() {
	return GlobalSettingsSingleton::Instance();
}
