#pragma once

#include "cinder/app/App.h"


#include "Singleton.h"
#include "Settings/SettingManager.h"
#include "Settings/SettingString.h"
#include "cinder/Text.h"
#include "cinder/gl/TextureFont.h"

class GlobalSettings {


public:

	// General
	SettingManager mSettingManager;

	Setting<bool>   debugMode;
	Setting<bool>   isFullscreen;
	Setting<bool>   hasWebcam;
	Setting<bool>   mirrorWebcam;
	Setting<bool>   isMouseOn;
	Setting<bool>   projectorMode;
	Setting<bool>   disablePenPressure;
	Setting<bool>   hideClearButton;
	Setting<bool>   hideSaveButton;
	Setting<bool>	isSpoutActive;
    Setting<bool>   isSyphonActive;

	Setting<int>    groupId;
	Setting<int>    nrOfFrames;


	GlobalSettings();

	Setting<int> frameWidth;
	Setting<int> frameHeight;
	Setting<int> resetScreenTime;
	Setting<float> screenScale;


	Setting<float> performanceDownScale;
	Setting<int> maxGroups;
	Setting<float> frameSpeed;



	void setup(std::string appName);


	Setting<float>  zoomLevel;
	ci::Font				mFont;
	ci::gl::TextureFontRef	mTextureFont;


	ci::ColorA fboBackground;


};



typedef Singleton<GlobalSettings> GlobalSettingsSingleton;

inline GlobalSettings* GS() {
	return GlobalSettingsSingleton::Instance();
}
