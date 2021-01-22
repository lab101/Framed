#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CinderImGui.h"
#include "cinder/Log.h"
#include "cinder/Filesystem.h"

#include "Helpers/GlobalSettings.h"

#if defined( CINDER_MSW_DESKTOP )
#include "CRTEventHandler.h"
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
	bool mTouchDown = false;

	std::vector<pointVec> mTest;

	po::scene::SceneRef     mScene;
	TouchUIRef mTouchUI;
	LineManager mLineManger;
	FrameManager mFrameManager;

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

	mTouchUI = TouchUI::create();
	mTouchUI->setup();
	mScene = po::scene::Scene::create(mTouchUI);

	mTest.reserve(100);

	mLineManger.onNewPoints.connect([=](pointVec points) {
		mFrameManager.drawPoints(points, mTouchUI->getColor());
		});

	mFrameManager.setup(4, getWindowSize());
	zoomCenterPoint.x = 200;
	zoomCenterPoint.y = 200;
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
	}
	else if (event.getCode() == event.KEY_s) {
		GS()->mSettingManager.writeSettings();
	}
	else if (event.getCode() == event.KEY_LEFT) {
		zoomAnchor.x -= 0.1;
	}
	if (event.getCode() == event.KEY_RIGHT) {
		zoomAnchor.x += 0.1;
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

	return g_Pressure;
}

void FramedApp::update()
{

	mScene->update();

	if (GS()->debugMode.value() && getElapsedFrames()) {
		//gl::enableAlphaBlending();
		//gl::enableAlphaBlendingPremult();
		//gl::color(Color::white());

		drawDebug();
		drawInfo();
	}


	mFrameManager.setFrameIndexNormalised(mTouchUI->getFrameScale());
}



void FramedApp::draw()
{
	gl::clear(Color(0.2, 0.2, 0.2));
	if (GS()->debugMode.value()) {
	}

	float pressure = 1.0f;//+ g_Pressure * 9.0f;

	const float margin = 20;
	Rectf activeArea(margin, margin, getWindow()->getWidth() - margin * 2, getWindow()->getHeight() - margin * 2);
	if (activeArea.contains(lastPenPosition)) {
		//   hideCursor();
	}
	else {
		showCursor();
	}

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

	gl::color(0.8, 0.8, 0);
	drawCursor(pressure, localCoordinate);

	// if(mTouchDown)  std::cout << "cursor " << localCoordinate.x << std::endl;


	 // get the screenmatrix when all the transformations on the "paper" (fbo) or done.
	screenMatrix = ci::gl::getModelViewProjection();

	ci::gl::popMatrices();

	mFrameManager.drawLoop();
	mFrameManager.drawGUI();

	mScene->draw();
	gl::color(0.8, 0.8, 0.8);
	//  drawCursor(pressure,lastPenPosition);


}

void FramedApp::drawCursor(float scale, vec2 position) const {
	float size = scale * 10;
	vec2 pointv2 = vec2(position.x, position.y);
	ci::gl::drawLine(pointv2 - ci::vec2(size, 0), pointv2 + ci::vec2(size, 0));
	ci::gl::drawLine(pointv2 + ci::vec2(0, -size), pointv2 + ci::vec2(0, +size));
}

void FramedApp::drawDebug()
{
}

void FramedApp::drawInfo()
{
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

CINDER_APP(FramedApp, RendererGl(RendererGl::Options().msaa(8)), [](App::Settings* settings) {
	settings->setWindowSize(1600, 1200);
	})
