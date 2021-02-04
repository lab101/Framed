
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

	frameWidth = Setting<int>("frameWidth", 1600);
	mSettingManager.addSetting(&frameWidth);

	frameHeight = Setting<int>("screenHeight", 1200);
	mSettingManager.addSetting(&frameHeight);

    frameSpeed = Setting<float>("frameSpeed", 4);
    mSettingManager.addSetting(&frameSpeed);


    zoomLevel = Setting<float>("zoomLevel", .8f);
	mSettingManager.addSetting(&zoomLevel);
    
    groupId = Setting<int>("groupId", 1);
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


