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
    Setting<bool>   isMouseOn;
    Setting<bool>   projectorMode;
    Setting<int>   groupId;


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

	Setting<float>  zoomLevel;


    ci::Font				mFont;
    ci::gl::TextureFontRef	mTextureFont;


    ci::ColorA fboBackground;


};



typedef Singleton<GlobalSettings> GlobalSettingsSingleton;

inline GlobalSettings* GS() {
	return GlobalSettingsSingleton::Instance();
}
