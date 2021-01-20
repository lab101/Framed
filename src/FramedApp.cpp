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

	ci::vec2 mousePos;
    bool mTouchDown = false;

	std::vector<pointVec> mTest;

    po::scene::SceneRef     mScene;
    TouchUIRef mTouchUI;
    LineManager mLineManger;

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

    mTouchUI = TouchUI::create();
    mTouchUI->setup();
    mScene = po::scene::Scene::create(mTouchUI);

    mTest.reserve(100);

    mLineManger.onNewPoints.connect([=] (pointVec points){

        mTest.push_back(points);


        for(auto& point: points){

           // vec2 p1 = vec2(point[i].x,point[i].y);
            std::cout << point << std::endl;
            gl::drawSolidCircle(point,2);
        //    i++;
        }
    });

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
	vec3 p(mousePos.x,mousePos.y,getPressure());
	mLineManger.newLine(p);

    mTouchDown = true;
}

void FramedApp::mouseMove(MouseEvent event)
{
	mousePos = event.getPos();

	if(mTouchDown){
        vec3 p(mousePos.x,mousePos.y,getPressure());
        mLineManger.lineTo(p,ci::Color::white());
    }
}

void FramedApp::mouseDrag(MouseEvent event)
{
	mousePos = event.getPos();
    vec3 p(mousePos.x,mousePos.y,getPressure());
    mLineManger.lineTo(p,ci::Color::white());
}

void FramedApp::mouseUp(MouseEvent event)
{
    if(mTouchDown){
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
        return g_pressure;
    #endif

    return 10;
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

}

void FramedApp::draw()
{
	gl::clear();
	if (GS()->debugMode.value()) {
	}

	float scale = 1.0f ;//+ g_Pressure * 9.0f;


    const float margin = 20;
    Rectf activeArea(margin,margin,getWindow()->getWidth() - margin*2,getWindow()->getHeight() - margin*2);
    if(activeArea.contains(mousePos)){
        hideCursor();
    }else{
        showCursor();
    }

    mScene->draw();

    ci::gl::color(0.8,0.8,0.8);
    float size = scale* 10;
    ci::gl::drawLine(mousePos - ci::vec2(size,0),mousePos + ci::vec2(size,0));
    ci::gl::drawLine(mousePos + ci::vec2(0,-size),mousePos + ci::vec2(0,+size));

    vec2 mSize = ci::app::getWindowSize();
    mSize.x = 1;
    mSize.y = 1;

    for(auto pointVec: mTest){
        int i = 0;
        for(auto point: pointVec){

            //    vec2 p1 = vec2(point[i].x,point[i].y);
          //  std::cout << p1 << std::endl;
            ci::gl::color(0.8,0.8,0.8 ,0.6);

            gl::drawSolidCircle(point,2);
            i++;
        }

    }



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
	settings->setWindowSize(1200, 1000);
	})
