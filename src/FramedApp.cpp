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

	vec3 lastPenPosition;
	float mPenPressure = 10;
	bool mTouchDown = false;

    
    vec2 frameSize = vec2(1600,1200);
    
	po::scene::SceneRef     mScene;
	TouchUIRef mTouchUI;
	LineManager mLineManger;
	FrameManager mFrameManager;
	NetworkManager mNetworkManager;

	// zoom related
	ci::mat4 screenMatrix;
	ci::vec3 localCoordinate;
	ci::vec2 zoomAnchor;
	void convertPointToLocalSpace(ci::vec3& point);
	vec3 getLocalPoint(vec3& screenPoint);
	int zoomDirection = 0;
	vec2 zoomCenterPoint;

	float mFps;
	void drawDebug();
	void drawInfo();

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
	cfg.SizePixels = 13 * SCALE;
	ImGui::GetIO().Fonts->AddFontDefault(&cfg)->DisplayOffset.y = SCALE;

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


	zoomCenterPoint.x = 420;
	zoomCenterPoint.y = 10;

    setupNetwork();


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

            for (auto &p : package.points) {
               // convertPointToLocalSpace(p);
                std::cout << "incoming"  <<  package.color << std::endl;

            }
//          s
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
	gl::clear(Color(0.42, 0.4, 0.4));
	if (GS()->projectorMode.value()) {
        mFrameManager.drawLoop(true);
        return;
    }

	float pressure = 1.0f;//+ g_Pressure * 9.0f;



	ci::gl::color(1, 1, 1);
	ivec2 size = mFrameManager.getSize();



	// Drawing "the paper" at zoomlevel with offset.
	ci::gl::pushMatrices();
	gl::ScopedViewport fbVP(getWindowSize());
	gl::setMatricesWindow(getWindowSize());
	ci::gl::translate(zoomCenterPoint.x, zoomCenterPoint.y, 0);

	float zoomLevel = 0.5 + mTouchUI->getScale();
	ci::gl::scale(zoomLevel, zoomLevel);
	ci::gl::translate(-size.x * zoomAnchor.x, -size.y * zoomAnchor.y, 0);
	mFrameManager.draw();

	gl::color(1, 1, 1, 0.2);

	mFrameManager.drawAtIndex(-1);

//	drawCursor(pressure, localCoordinate);


	 // get the screenmatrix when all the transformations on the "paper" (fbo) or done.
	screenMatrix = ci::gl::getModelViewProjection();

	ci::gl::popMatrices();
	ci::gl::color(1, 1, 1);

	mFrameManager.drawLoop();

    ci::gl::color(1, 1, 1);

    mScene->draw();
    drawCursor(pressure,lastPenPosition);

	mTouchUI->draw();



    if (GS()->debugMode.value() && getElapsedFrames()) {
        drawDebug();
    }



}

void FramedApp::drawCursor(float scale, vec2 position) const {

    gl::color(0.8, 0.8, 0.8);

    float size = scale * 10;
	vec2 pointv2 = vec2(position.x, position.y);
	ci::gl::drawLine(pointv2 - ci::vec2(size, 0), pointv2 + ci::vec2(size, 0));
	ci::gl::drawLine(pointv2 + ci::vec2(0, -size), pointv2 + ci::vec2(0, +size));
}

void FramedApp::drawDebug()
{
    mFrameManager.drawGUI();

    
    mFps = getAverageFps();

    ImGui::Begin("Settings");
    ImGui::Text("framerate: %f", mFps);
    ImGui::Checkbox("show debug", &GS()->debugMode.value());
    if (ImGui::Button("load setttingset 1")) {
        GS()->loadSettingSet1();
    }
    ImGui::Separator();
    if (ImGui::Button("sav setttings")) {
        GS()->mSettingManager.writeSettings();
    }
    ImGui::End();
}

void FramedApp::drawInfo()
{
    

}

CINDER_APP(FramedApp, RendererGl(RendererGl::Options().msaa(0)), [](App::Settings* settings) {
	settings->setWindowSize(1600, 1200);
	})
