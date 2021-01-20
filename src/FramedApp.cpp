#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CinderImGui.h"
#include "cinder/Log.h"
#include "cinder/Filesystem.h"

#include "Helpers/GlobalSettings.h"
#include "CRTEventHandler.h"

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

	void activatePenPressure();
private:

	ci::vec2 mousePos;

	float mFps;
	void drawDebug();
	void drawInfo();

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

}


void FramedApp::setupLogging() {
	// LOGGING to screen (notifications) and disk
	std::string const logFolder = ci::getDocumentsDirectory().string() + "/logs";
	if (!ci::fs::exists(logFolder)) {
		ci::fs::create_directory(logFolder);
	}

	ci::log::makeLogger<ci::log::LoggerFileRotating>(logFolder, appName + ".%Y.%m.%d.log", true);
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

}

void FramedApp::mouseDown(MouseEvent event)
{
	mousePos = event.getPos();
}

void FramedApp::mouseMove(MouseEvent event)
{
	mousePos = event.getPos();
}

void FramedApp::mouseDrag(MouseEvent event)
{
	mousePos = event.getPos();
}

void FramedApp::mouseUp(MouseEvent event)
{
	activatePenPressure();
}

void FramedApp::activatePenPressure() {

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

}

void FramedApp::update()
{


	if (GS()->debugMode.value() && getElapsedFrames()) {
		//gl::enableAlphaBlending();
		//gl::enableAlphaBlendingPremult();
		//gl::color(Color::white());

		drawDebug();
		drawInfo();
	}

}

void FramedApp::draw()
{
	gl::clear();
	if (GS()->debugMode.value()) {
	}

	float scale = 1.0f + g_Pressure * 9.0f;


	ci::gl::drawSolidCircle(mousePos, scale * 10);
	ci::gl::drawString(std::to_string(scale), ci::vec2(100, 200));


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

CINDER_APP(FramedApp, RendererGl(RendererGl::Options().msaa(0)), [](App::Settings* settings) {
	settings->setWindowSize(2000, 1400);
	})
