
#include <stdio.h>
#include "GlobalSettings.h"
#include "cinder/Utilities.h"

using namespace std;
using namespace ci;



void GlobalSettings::setup(std::string appName) {

	mSettingManager.setAppName(appName);
	mSettingManager.readSettings();

	debugMode = Setting<bool>("debug", false);
	mSettingManager.addSetting(&debugMode);

	isMouseOn = Setting<bool>("isMouseOn", true);
	mSettingManager.addSetting(&isMouseOn);

	isFullscreen = Setting<bool>("isFullscreen", false);
	mSettingManager.addSetting(&isFullscreen);

	screenScale = Setting<float>("screenScale", 1.0).setSteps(0.001f);
	screenScale.setMax(2.0f);
	screenScale.setMin(.1f);
	mSettingManager.addSetting(&screenScale);

	screenWidth = Setting<int>("screenwidth", 1080);
	mSettingManager.addSetting(&screenWidth);

	screenHeight = Setting<int>("screenHeight", 1080);
	mSettingManager.addSetting(&screenHeight);

    zoomLevel = Setting<float>("zoomLevel", .8f);
	mSettingManager.addSetting(&zoomLevel);
    
    groupId = Setting<int>("groupId",0);
    mSettingManager.addSetting(&groupId);
    
    
    projectorMode = Setting<bool>("projectorMode", false);
    mSettingManager.addSetting(&projectorMode);


    fboBackground = ColorA(0.0,0.0,0.0,1.0);

    // fonts
#if defined( CINDER_COCOA )
    mFont = Font( "BigCaslon-Medium", 24 );
#elif defined( CINDER_LINUX )
    mFont = Font( "Times New Roman", 24 );
#else
    mFont = Font( "Times New Roman", 48 );
#endif

mFont = Font(ci::app::loadAsset("fonts/PTSans-Regular.ttf") , 12);

    mTextureFont = gl::TextureFont::create( mFont );
}



GlobalSettings::GlobalSettings() {
    performanceDownScale.value() = 1.0;
}

ci::vec2 GlobalSettings::getScreenSize() {
	return ci::vec2(screenWidth.value(), screenHeight.value());
}


ci::vec2 GlobalSettings::getScreenCenter() {
	return ci::vec2(screenWidth.value() * 0.5f, screenHeight.value() * 0.5f);
}




