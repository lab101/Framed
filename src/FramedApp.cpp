#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CinderImGui.h"
#include "cinder/Log.h"
#include "cinder/Filesystem.h"
#include "Helpers/NetworkManager.h"

#include "Helpers/GlobalSettings.h"

#if defined( CINDER_MSW_DESKTOP )
#include "Pen/Windows/CRTEventHandler.h"
#endif

#if defined( CINDER_COCOA )
#include "Pen/OSX/ofxTablet.h"
#endif

#include "poScene/Scene.h"
#include "UI/TouchUI.h"
#include "Helpers/LineManager.h"
#include "Helpers/FrameManager.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class FramedApp : public App {
public:
	void setup() override;
	void setupLogging();
	void keyDown(KeyEvent event) override;
	void mouseDown(MouseEvent event) override;
	void mouseDrag(MouseEvent event) override;
	void mouseUp(MouseEvent event) override;
	void mouseMove(MouseEvent event) override;
	void update() override;
	void draw() override;

	float getPressure();
	void activatePenPressure();


private:

	ci::vec3 lastPenPosition;
    ci::vec3 localCoordinate;

	float mPenPressure = 10;
	bool mTouchDown = false;

    
    ci::vec2 frameSize = vec2(1600,1200);
    
	po::scene::SceneRef     mScene;
	TouchUIRef mTouchUI;
	LineManager mLineManger;
	FrameManager mFrameManager;
	NetworkManager mNetworkManager;


	// zoom related
	ci::mat4 screenMatrix;
	ci::vec2 zoomAnchor;
	vec3 getLocalPoint(vec3& screenPoint);
	int zoomDirection = 0;
	vec2 zoomCenterPoint;

	float mFps;
	void drawDebug();
    void drawInterface();
	void drawCursor(float scale, vec2 position) const;

    void setupNetwork();
};

std::string appName = "Framed";


void FramedApp::setup()
{

	GS()->setup(appName);

	ImGui::Initialize();
	ImGui::StyleColorsClassic();
	float SCALE = 1.0f;
	ImFontConfig cfg;
	cfg.SizePixels = 20 * SCALE;
	ImGui::GetIO().Fonts->AddFontDefault(&cfg)->DisplayOffset.y = SCALE;

	frameSize = vec2(GS()->frameWidth.value(),GS()->frameHeight.value());
	mFrameManager.setup(6, frameSize);

	mTouchUI = TouchUI::create();
	mTouchUI->setup();
	mTouchUI->addThumbs(mFrameManager.getTextures());
	mTouchUI->setActiveFrame(0);
    
    mTouchUI->onErase.connect([=]{
        mNetworkManager.sendErase();
    });

	mScene = po::scene::Scene::create(mTouchUI);


	mLineManger.onNewPoints.connect([=](pointVec points) {
		mFrameManager.drawPoints(points, mTouchUI->getColor());
        mNetworkManager.sendPoints(points, false, mTouchUI->getColor(), mFrameManager.getActiveFrame());
	});


	mNetworkManager.onErase.connect([=](){
        mFrameManager.saveAll();
        mFrameManager.clearAll();
	});


	zoomCenterPoint.x = 410;
	zoomCenterPoint.y = 10;

    setupNetwork();

    if(GS()->projectorMode.value()){
        hideCursor();
    }

#if defined( CINDER_COCOA )
	CI_LOG_I("START ofxTablet");
	ofxTablet::start();
	ofxTablet::onData.connect([=] (TabletData& data) {
	    mPenPressure = data.pressure * 20;
	});
	CI_LOG_I("finished ofxTablet");
#endif
}

void FramedApp::setupNetwork() {

  //  mNetworkManager.setup();

    // NETWORK SETUP
    if(mNetworkManager.setup()) {
        // points
        mNetworkManager.onReceivePoints.connect([=](PointsPackage package) {
            //bool currentEraser = BrushManagerSingleton::Instance()->isEraserOn;
            //BrushManagerSingleton::Instance()->isEraserOn = package.isEraserOn;
            mFrameManager.drawPoints(package.points, package.color, package.frameId);
        });
    }
}


void FramedApp::setupLogging() {
	// LOGGING to screen (notifications) and disk
	std::string const logFolder = ci::getDocumentsDirectory().string() + "/logs";
	if (!ci::fs::exists(logFolder)) {
		ci::fs::create_directory(logFolder);
	}

	ci::log::makeLogger<ci::log::LoggerFileRotating>(logFolder, appName + ".%Y.%m.%d.log", true);
}


// some parts based on http://discourse.libcinder.org/t/screen-to-world-coordinates/1014/2
vec3 FramedApp::getLocalPoint(vec3& screenPoint) {

	int w = ci::app::getWindowWidth();
	int h = ci::app::getWindowHeight();

	vec4 viewport = vec4(0, h, w, -h); // vertical flip is required
	vec3 localPoint = glm::unProject(screenPoint, mat4(), screenMatrix, viewport);
	localPoint.z = screenPoint.z;

	return localPoint;
}

void FramedApp::keyDown(KeyEvent event)
{
	if (event.getChar() == 'd') {
		GS()->debugMode.setValue(!GS()->debugMode.value());
        if(GS()->debugMode.value()){
            showCursor();
        }else{
            hideCursor();
        }
	}
	else if (event.getCode() == event.KEY_f) {
		setFullScreen(!isFullScreen());
        if(isFullScreen()) hideCursor();
        else showCursor();
	}
	else if (event.getCode() == event.KEY_s) {
		GS()->mSettingManager.writeSettings();
	}
    else if (event.getCode() == event.KEY_p) {
        GS()->projectorMode.setValue(!GS()->projectorMode.value());
    }
	else if (event.getCode() == event.KEY_LEFT) {
		zoomAnchor.x -= 0.1;
	}
	if (event.getCode() == event.KEY_RIGHT) {
		zoomAnchor.x += 0.1;
	}
	if (event.getCode() == event.KEY_SPACE) {
		mFrameManager.saveAll();
		mFrameManager.clearAll();
		mTouchUI->setActiveFrame(0);
	}

}

void FramedApp::mouseDown(MouseEvent event)
{
	lastPenPosition = vec3(event.getPos(), getPressure());
	localCoordinate = getLocalPoint(lastPenPosition);

	mLineManger.newLine(localCoordinate);
	mTouchDown = true;
}

void FramedApp::mouseMove(MouseEvent event)
{
	lastPenPosition = vec3(event.getPos().x, event.getPos().y, getPressure());

	if (mTouchDown) {
		localCoordinate = getLocalPoint(lastPenPosition);
		mLineManger.lineTo(localCoordinate, ci::Color::white());
	}
}

void FramedApp::mouseDrag(MouseEvent event)
{
	if (mTouchDown) {
		lastPenPosition = vec3(event.getPos(), getPressure());
		localCoordinate = getLocalPoint(lastPenPosition);

		mLineManger.lineTo(localCoordinate, ci::Color::white());
	}
}

void FramedApp::mouseUp(MouseEvent event)
{

	lastPenPosition = vec3(event.getPos(), 10);

	if (mTouchDown) {
		activatePenPressure();
		mLineManger.endLine();
	}
	mTouchDown = false;
}

void FramedApp::activatePenPressure() {

#if defined( CINDER_MSW_DESKTOP )
	if (!g_bTriedToCreateRTSHandler) {
		if (ci::app::getElapsedFrames() > 1) {
			HWND hw = (HWND)ci::app::getWindow()->getNative();
			bool result = CreateRTS(hw);

			if (result) {
				g_bTriedToCreateRTSHandler = true;
				hideCursor();
			}
		}
	}
#endif
}

float FramedApp::getPressure() {
#if defined( CINDER_MSW_DESKTOP )
	return g_Pressure * 20.0;
#endif

	return mPenPressure;
}

void FramedApp::update()
{

    mNetworkManager.update();
    mScene->update();

	mFrameManager.setFrameIndexNormalised(mTouchUI->getFrameScale());
	mTouchUI->updateThumbs(mFrameManager.getTextures());
	mTouchUI->onFrameSlected.connect([=](int id) {
		mFrameManager.setActiveFrame(id);
	});
}



void FramedApp::draw()
{
	gl::clear(Color(0.2, 0.2, 0.25));
	if (GS()->projectorMode.value()) {
        mFrameManager.drawLoop(true);
    }else{
        drawInterface();
    }


    if (GS()->debugMode.value()) {
        drawDebug();
    }
}


void FramedApp::drawInterface(){
        ci::gl::color(1, 1, 1);
        ivec2 size = mFrameManager.getSize();

        // Drawing "the paper" at zoomlevel with offset.
        ci::gl::pushMatrices();
        gl::ScopedViewport fbVP(getWindowSize());
        gl::setMatricesWindow(getWindowSize());
        ci::gl::translate(zoomCenterPoint.x, zoomCenterPoint.y, 0);

        float zoomLevel = 0.5 + mTouchUI->getScale();

        // make less hardcoded later.
        float adjustForAvailableSpace = (float) (getWindowWidth() -420) / (float) (size.x) ;

        ci::gl::scale(adjustForAvailableSpace, adjustForAvailableSpace);
        ci::gl::translate(-size.x * zoomAnchor.x, -size.y * zoomAnchor.y, 0);
        mFrameManager.draw();

        gl::color(1, 1, 1, 0.2);

        mFrameManager.drawAtIndex(-1);

         // get the screen matrix when all the transformations on the "paper" (fbo) or done.
        screenMatrix = ci::gl::getModelViewProjection();

        ci::gl::popMatrices();
        ci::gl::color(1, 1, 1);

        mFrameManager.drawLoop();

        ci::gl::color(1, 1, 1);

        mScene->draw();
        drawCursor(getPressure(),lastPenPosition);

        mTouchUI->draw();

}

void FramedApp::drawCursor(float scale, vec2 position) const {

    gl::color(0.8, 0.8, 0.8);

    float size = scale * 2;
	vec2 pVec2 = vec2(position.x, position.y);
	ci::gl::drawLine(pVec2 - ci::vec2(size, 0), pVec2 + ci::vec2(size, 0));
	ci::gl::drawLine(pVec2 + ci::vec2(0, -size), pVec2 + ci::vec2(0, +size));
}

void FramedApp::drawDebug()
{
    mFrameManager.drawGUI();

    mFps = getAverageFps();

    ImGui::Begin("Settings");
    ImGui::Text("framerate: %f", mFps);
    ImGui::Checkbox("show debug", &GS()->debugMode.value());
    ImGui::Separator();
    ImGui::Checkbox("projector mode", &GS()->projectorMode.value());
    ImGui::Checkbox("fullscreen", &GS()->isFullscreen.value());
    if(ImGui::SliderInt("group id", &GS()->groupId.value(),1,4)){
        mNetworkManager.setGroupId(GS()->groupId.value());
    }
    
    if (ImGui::Button("save setttings")) {
        GS()->mSettingManager.writeSettings();
    }
    ImGui::End();
}


CINDER_APP(FramedApp, RendererGl(RendererGl::Options().msaa(0)), [](App::Settings* settings) {
	settings->setWindowSize(1600, 1200);
	})
