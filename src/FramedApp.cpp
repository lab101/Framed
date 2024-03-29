#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CinderImGui.h"
#include "cinder/Log.h"
#include "cinder/Filesystem.h"
#include "Helpers/NetworkManager.h"
#include "Helpers/GlobalSettings.h"
#include "mathHelper.h"
#include "enum.h"

#if defined( CINDER_MSW_DESKTOP )
#include "Pen/Windows/CRTEventHandler.h"
#include "CiSpoutOut.h"
#endif

#if defined( CINDER_COCOA )
#include "Pen/OSX/ofxTablet.h"
#endif

#include "poScene/Scene.h"
#include "UI/TouchUI.h"
#include "Helpers/LineManager.h"
#include "Helpers/FrameManager.h"
#include "Helpers/OverlayManager.h"
#include "Helpers/TemplateManager.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class FramedApp : public App {

public:
	void setup() override;
	void setupLogging();
	void keyDown(KeyEvent event) override;
    void keyUp(KeyEvent event) override;
	void mouseDown(MouseEvent event) override;
	void mouseDrag(MouseEvent event) override;
	void mouseUp(MouseEvent event) override;
	void mouseMove(MouseEvent event) override;

	void fileDrop(FileDropEvent event) override;
	void processMove(MouseEvent event);
	void update() override;
	void draw() override;
	void drawShapes();
	void eraseAndSave();
	void save();
	float getPressure();
	void activatePenPressure();


private:

#ifdef CINDER_MSW
	// spout
	SpoutOut* mSpoutOut = nullptr;
#endif

	ci::vec3 lastPenPosition;
	ci::vec3 localCoordinate;
	ci::Color backgroundColor;
	float mPenPressure = 0.25;
	bool mTouchDown = false;
	bool useOverLay = false;
	bool isSetupComplete = false;
    

    float lastUpdateTime = 0;
    // used for the paper function;
    vec2 penMoveStart;


	ToolState mCurrentToolState;
	ci::vec2 mShapeStartPoint;
	ci::vec2 mShapeEndPoint;


	std::queue<PointsPackage> packageQueue;
	std::mutex pLock;

	ci::vec2 frameSize = vec2(GS()->frameWidth.value(), GS()->frameHeight.value());

	po::scene::SceneRef     mScene;
	TouchUIRef mTouchUI;
	LineManager mLineManger;
	FrameManager mFrameManager;
	OverlayManager mOverlayManager;
	TemplateManager mTemplateManager;
	// this was changed due a threading bug which in the end wasn't
	// should be changed back to non pointer.
	NetworkManager* mNetworkManager;

	// zoom related
	ci::mat4 screenMatrix;
	ci::vec2 zoomAnchor;
	vec3 getLocalPoint(vec3& screenPoint);
	int zoomDirection = 0;
	vec2 zoomCenterPoint;
    bool isMovingPaper = false;

	float mFps;
    void setNewFrameSize();
    
    void drawDebug();
	void drawInterface();
	void drawCursor(float scale, vec2 position) const;

	void setupNetwork();
	void setupImGui();
	void setupSpout();
	// change the nr of frames locally and over the network
	void changeNrOfFramesGlobal(int newNrOfFrames);
	void changeColorGlobal(ci::Color color);
};

std::string appName = "Framed";


void FramedApp::setup()
{

	GS()->setup(appName);
    auto f = ci::app::getWindow()->getContentScale();
    //::getWindowContentScale();
    
	frameSize = vec2(GS()->frameWidth.value(), GS()->frameHeight.value());

	mTouchUI = TouchUI::create();
	mTouchUI->setup(400 * frameSize.y / frameSize.x);
	mTouchUI->setActiveFrame(0);

	setupImGui();
    
	backgroundColor = Color(0.2, 0.2, 0.25);

    
	mFrameManager.setup(GS()->nrOfFrames.value(), frameSize);
	mOverlayManager.setup(GS()->nrOfFrames.value(), frameSize);
	mTemplateManager.setup();
	mFrameManager.drawTextures(mTemplateManager.getTextures());


	// erase
	mTouchUI->onErase.connect([=] {
		eraseAndSave();
		});

	// save
	mTouchUI->onSave.connect([=] {
		save();
		});


	// tool change
	mTouchUI->onNewToolElection.connect([=](ToolState state) {
		mCurrentToolState = state;
		});

	mScene = po::scene::Scene::create(mTouchUI);

	// incoming points from the user.
	mLineManger.onNewPoints.connect([=](pointVec points) {
		mFrameManager.drawPoints(points, mTouchUI->getColor(), mFrameManager.getActiveFrame());

		if (mNetworkManager) {
			mNetworkManager->sendPoints(points, false, mTouchUI->getColor(), mFrameManager.getActiveFrame());
		}
		});

//	zoomCenterPoint.x = 410;
//	zoomCenterPoint.y = 10;
    
    zoomCenterPoint = ci::app::getWindowCenter();
    zoomAnchor = vec2(0.5,0.5);



	if (GS()->projectorMode.value()) {
		hideCursor();
	}

#if defined( CINDER_COCOA )
	CI_LOG_I("START ofxTablet");
    // we only use the pressure.
    // x,y we take from the mouse events which makes it easier
    // to maintain between platforms
    
	ofxTablet::start();
	ofxTablet::onData.connect([=](TabletData& data) {
		mPenPressure = data.pressure;
		});
	CI_LOG_I("finished ofxTablet");
#endif


	setupSpout();

	mCurrentToolState = ToolState::BRUSH;

	setupNetwork();

	setFullScreen(GS()->isFullscreen.value());

	isSetupComplete = true;

}

void FramedApp::setupSpout() {
#ifdef CINDER_MSW
	// SPOUT
	if (GS()->isSpoutActive.value() && mSpoutOut == nullptr) {
		mSpoutOut = new ci::SpoutOut("FRAMED", frameSize);
	}
#endif
}

void FramedApp::eraseAndSave() {
	console() << "CLEAR" << std::endl;
	mFrameManager.saveAll();
	mFrameManager.clearAll();
	mTouchUI->setActiveFrame(0);
}

void FramedApp::save() {
	mFrameManager.saveAll();
}

void FramedApp::setupNetwork() {

	mNetworkManager = new NetworkManager();
	if (mNetworkManager->setup()) {
		// points
		mNetworkManager->onReceivePoints.connect([=](PointsPackage package) {
			if (package.shape == BRUSH) {
				mFrameManager.drawPoints(package.points, package.color, package.frameId);
			}
			else if (package.shape == CIRCLE) {
				mFrameManager.drawCircle(package.points[0], package.points[1], package.color, package.frameId);
			}
			else if (package.shape == RECTANGLE) {
				mFrameManager.drawRectangle(package.points[0], package.points[1], package.color, package.frameId);
			}

			});

		mNetworkManager->onErase.connect([=]() {
			eraseAndSave();
			});

		mNetworkManager->onNumberOfFramesChanged.connect([=](int nrOfFramesChanged) {
			mFrameManager.changeNrOfFrames(nrOfFramesChanged);
			});

		mNetworkManager->onFrameSpeedChanged.connect([=](int framespeedChanged) {
			GS()->frameSpeed.setValue(framespeedChanged);
			});
        
        mNetworkManager->onFrameSizeChanged.connect([=](ci::vec2 newFrameSize) {
           // GS()->frameSpeed.setValue(framespeedChanged);
            std::cout << newFrameSize;
            frameSize = newFrameSize;
            mFrameManager.setup(GS()->nrOfFrames.value(), newFrameSize);
            
            setNewFrameSize();
            
        });


		// set group on startup
		mNetworkManager->setGroupId(GS()->groupId.value());

	}

}


void FramedApp::setupImGui() {
	ImGui::Initialize();
	ImGui::StyleColorsClassic();
    float SCALE = ci::app::getWindow()->getContentScale();
	ImFontConfig cfg;
	cfg.SizePixels = 14 * SCALE;
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
    
    bool calculateAnchor =false;

	if (event.getChar() == 'd') {
		GS()->debugMode.setValue(!GS()->debugMode.value());
		if (GS()->debugMode.value()) {
			showCursor();
		}
		else {
			hideCursor();
		}
	}
	else if (event.getCode() == event.KEY_f) {
		setFullScreen(!isFullScreen());
		if (isFullScreen()) hideCursor();
		else showCursor();
	}
	else if (event.getCode() == event.KEY_x) {
		// send a clear to the whole network!
		eraseAndSave();
		mNetworkManager->sendErase();
	}
    
    else if(event.getCode() == event.KEY_v){
           
           zoomDirection = 1;
           calculateAnchor = true;
       }
       else if(event.getCode() == event.KEY_n){
           zoomDirection = -1;
           calculateAnchor = true;
       }
	else if (event.getCode() == event.KEY_s) {
		GS()->mSettingManager.writeSettings();
	}
	else if (event.getCode() == event.KEY_p) {
		GS()->projectorMode.setValue(!GS()->projectorMode.value());
	}
	else if (event.getCode() == event.KEY_LEFT) {
		mFrameManager.prevFrame();
		mTouchUI->setActiveFrame(mFrameManager.getActiveFrame());
	}
	else if (event.getCode() == event.KEY_RIGHT) {
		mFrameManager.nextFrame();
		mTouchUI->setActiveFrame(mFrameManager.getActiveFrame());
	}
	else if (event.getCode() == event.KEY_UP) {
		mFrameManager.prevFrame();
		mTouchUI->setActiveFrame(mFrameManager.getActiveFrame());
	}
	else if (event.getCode() == event.KEY_DOWN) {
		mFrameManager.nextFrame();
		mTouchUI->setActiveFrame(mFrameManager.getActiveFrame());
	}
    else if(!isMovingPaper&&  event.getCode() == event.KEY_SPACE ){
           isMovingPaper = true;
           penMoveStart = vec2(lastPenPosition.x,lastPenPosition.y);
    }
//    else if (event.getCode() == event.KEY_SPACE) {
//        mOverlayManager.setActiveFrame(mFrameManager.getActiveFrame());
//        mOverlayManager.snap();
//    }
    else if (event.getChar() == '[') {
        GS()->frameSpeed.increaseStep(1);
        mNetworkManager->setFrameSpeed(GS()->frameSpeed.value());
    }
    else if (event.getChar() == ']') {
        if(GS()->frameSpeed.value() > 0){
            GS()->frameSpeed.decreaseStep(1);
            mNetworkManager->setFrameSpeed(GS()->frameSpeed.value());
        }
    }

	// director keys
	else if (event.getCode() == event.KEY_1) {
		int newFrameNr = 1;
		Color c(1, 0, 0.2);

		changeNrOfFramesGlobal(newFrameNr);
		changeColorGlobal(c);

	}
	else if (event.getCode() == event.KEY_2) {
		int newFrameNr = 6;
		Color c(1, 1, 1);

		changeNrOfFramesGlobal(newFrameNr);
		changeColorGlobal(c);
	}

	else if (event.getCode() == event.KEY_3) {
		changeNrOfFramesGlobal(10);

	}
	else if (event.getCode() == event.KEY_4) {
		changeNrOfFramesGlobal(10);
		mOverlayManager.setupCamera();
		mOverlayManager.isLive = true;
		useOverLay = true;
	}
	else if (event.getCode() == event.KEY_5) {
		changeNrOfFramesGlobal(10);
		mOverlayManager.stopCamera();
		mOverlayManager.isLive = false;
		useOverLay = false;
	}

	else if (event.getCode() == event.KEY_6) {
		changeNrOfFramesGlobal(2);

	}

    
    
    if(calculateAnchor){
        ivec2 size = mFrameManager.getActiveTexture()->getSize();
        
        zoomCenterPoint = vec2(lastPenPosition);
        vec3 localPointCapped = localCoordinate;
        localPointCapped.x = fmax(0,fmin(localPointCapped.x,size.x));
        localPointCapped.y = fmax(0,fmin(localPointCapped.y,size.y));
        
        zoomAnchor = vec2(localPointCapped.x / size.x , localPointCapped.y / size.y);
    }
}


void FramedApp::keyUp(KeyEvent event ){
    
    zoomDirection = 0;
    
    if(event.getCode() == event.KEY_SPACE){
        isMovingPaper = false;
    }
}


void FramedApp::changeColorGlobal(ci::Color color) {
	auto s = mFrameManager.getSize();
	int frameCount = mFrameManager.totalFrameCount();

	for (int i = 0; i < frameCount; i++) {
        ci::vec3 start(0,0,0);
        ci::vec3 end(s.x, s.y, 0);
		mNetworkManager->sendTwoPointShape(start, end, ToolState::RECTANGLE, color,
			i);
		mFrameManager.drawRectangle(vec3(0, 0, 0), vec3(s.x, s.y, 0), color,
			i);
	}
}

void FramedApp::changeNrOfFramesGlobal(int newNrOfFrames) {
	mNetworkManager->setNrOfFrames(newNrOfFrames);
	mNetworkManager->sendErase();
	mFrameManager.clearAll();
	mFrameManager.changeNrOfFrames(newNrOfFrames);
}

void FramedApp::fileDrop(FileDropEvent event)
{
	try {
		auto mTexture = gl::Texture::create(loadImage(loadFile(event.getFile(0))));
		mOverlayManager.setTexture(mFrameManager.getActiveFrame(), mTexture);
	}
	catch (Exception& exc) {
		CI_LOG_EXCEPTION("failed to load image: " << event.getFile(0), exc);
	}
}

void FramedApp::mouseDown(MouseEvent event)
{
    if(isMovingPaper)
    {
        vec2 p2 = vec2(lastPenPosition.x,lastPenPosition.y);
        penMoveStart = p2;
        
    }else{

        lastPenPosition = vec3(event.getPos(), getPressure());
        localCoordinate = getLocalPoint(lastPenPosition);
        if (localCoordinate.x < 0) return;

        mTouchDown = true;

        switch (mCurrentToolState) {
        case ToolState::BRUSH:
            mLineManger.newLine(localCoordinate);
            break;
        case ToolState::RECTANGLE:
        case ToolState::CIRCLE:
            mShapeStartPoint = vec2(localCoordinate.x, localCoordinate.y);
            mShapeEndPoint = mShapeStartPoint;
            break;
        }
    }
}

void FramedApp::mouseMove(MouseEvent event)
{
	processMove(event);
}

void FramedApp::mouseDrag(MouseEvent event)
{
	processMove(event);
}

void FramedApp::processMove(MouseEvent event) {

	// sometimes the events are triggered when the app is not fully loaded
	if (!isSetupComplete) return;
    
    lastPenPosition = vec3(event.getPos().x, event.getPos().y, getPressure());
    localCoordinate = getLocalPoint(lastPenPosition);

    if(isMovingPaper){
          vec2 p2 = vec2(lastPenPosition.x,lastPenPosition.y);
          vec2 div =(penMoveStart - p2) ;
          zoomCenterPoint -=div;
          penMoveStart = p2;
          
    }else{


        if (mTouchDown) {
                localCoordinate = getLocalPoint(lastPenPosition);

                switch (mCurrentToolState) {
                case ToolState::BRUSH:
                    mLineManger.lineTo(localCoordinate, mTouchUI->getColor());
                    break;
                case ToolState::RECTANGLE:
                case ToolState::CIRCLE:
                    mShapeEndPoint = vec2(localCoordinate.x, localCoordinate.y);
                    break;
                }
            }
        }
}

void FramedApp::mouseUp(MouseEvent event)
{

    if(isMovingPaper) return;

	lastPenPosition = vec3(event.getPos(), getPressure());

	if (mTouchDown) {
		activatePenPressure();

		switch (mCurrentToolState) {
		case ToolState::BRUSH:
			mLineManger.endLine();
			break;
		case ToolState::LINE:
			//mFrameManager.drawCircle(mShapeStartPoint, mShapeEndPoint, mTouchUI->getColor());
			break;
		case ToolState::CIRCLE: {
			mFrameManager.drawCircle(mShapeStartPoint, mShapeEndPoint, mTouchUI->getColor(),
				mFrameManager.getActiveFrame());

			vec3 p1(mShapeStartPoint.x, mShapeStartPoint.y, getPressure());
			vec3 p2(mShapeEndPoint.x, mShapeEndPoint.y, getPressure());
			mNetworkManager->sendTwoPointShape(p1, p2, ToolState::CIRCLE, mTouchUI->getColor(),
				mFrameManager.getActiveFrame());
			break;
		}
		case ToolState::RECTANGLE: {
			mFrameManager.drawRectangle(mShapeStartPoint, mShapeEndPoint, mTouchUI->getColor(),
				mFrameManager.getActiveFrame());

			vec3 p1(mShapeStartPoint.x, mShapeStartPoint.y, getPressure());
			vec3 p2(mShapeEndPoint.x, mShapeEndPoint.y, getPressure());
			mNetworkManager->sendTwoPointShape(p1, p2, ToolState::RECTANGLE, mTouchUI->getColor(),
				mFrameManager.getActiveFrame());
			break;
		}
		}
	}
	mTouchDown = false;
}

void FramedApp::activatePenPressure() {

	// disable the pen pressure if it would give performance issues
	if (GS()->disablePenPressure.value()) return;


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

	const float scale = lab101::ofMap(mTouchUI->getStrokeScale(), 0, 1, 12, 140);

#if defined( CINDER_MSW_DESKTOP )
	if (g_Pressure > 0) mPenPressure = g_Pressure;
#endif

	return mPenPressure * scale;
}

void FramedApp::update()
{
    if (GS()->debugMode.value()) {
        drawDebug();
    }

    mFrameManager.update();
	mNetworkManager->update();
	if (useOverLay) mOverlayManager.update();
	mScene->update();

	mTouchUI->updateThumbs(mFrameManager.getTextures());
	mTouchUI->onFrameSlected.connect([=](int id) {
		mFrameManager.setActiveFrame(id);
		});


	//	pLock.lock();
	//	if (!packageQueue.empty()) {
	//		auto package = packageQueue.front();
	//		packageQueue.pop();
	//		mFrameManager.drawPoints(package.points, package.color, package.frameId);
	//	}
	//	pLock.unlock();
    
    // provides smooth in & out zoom
    
    const float div = ci::app::getElapsedSeconds() - lastUpdateTime;
    lastUpdateTime = ci::app::getElapsedSeconds();

    if(zoomDirection != 0){
        
        if(fabs(zoomDirection) < 2){
            zoomDirection *= 2.0;
        }
        
        GS()->zoomLevel.value() += zoomDirection * div * 0.7;
        if(GS()->zoomLevel.value() < 0.1) GS()->zoomLevel.value() = 0.1;
    }
    

}



void FramedApp::draw()
{
    gl::clear(backgroundColor);

    
	gl::clear(Color(0.2, 0.2, 0.25));
	if (GS()->projectorMode.value()) {
		mFrameManager.drawLoop(true);
	}
	else {
		drawInterface();
	}


#if defined( CINDER_MSW_DESKTOP )
	if (GS()->isSpoutActive.value()) mSpoutOut->sendTexture(mFrameManager.getLoopTexture());
#endif

}


void FramedApp::drawInterface() {
	ci::gl::color(1, 1, 1);
	ivec2 frameSize = mFrameManager.getSize();

    {
        // Drawing "the paper" at zoomlevel with offset.
        ci::gl::pushMatrices();
       // gl::ScopedViewport fbVP(getWindowSize());
       // gl::setMatricesWindow(getWindowSize());
        ci::gl::translate(zoomCenterPoint.x, zoomCenterPoint.y, 0);
       //   std::cout << zoomCenterPoint.x <<std::endl;
       // float zoomLevel =1;// 0.5 + mTouchUI->getScale();

        // make less hardcoded later.
        float adjustForAvailableSpace = (float)(getWindowWidth() - 420) / (float)(frameSize.x);

        ci::gl::scale(adjustForAvailableSpace*GS()->zoomLevel.value(), adjustForAvailableSpace*GS()->zoomLevel.value());
        ci::gl::translate(-frameSize.x * zoomAnchor.x, -frameSize.y * zoomAnchor.y, 0);
        //std::cout << frameSize.x << std::endl;
        
        mFrameManager.draw();

        gl::color(1, 1, 1, 0.2);
        mFrameManager.drawAtIndex(-1);

        // overlay
        if (useOverLay) {
            gl::color(1, 1, 1, 0.4);
            mOverlayManager.drawAtIndex(mFrameManager.getActiveFrame());
        }

        // draw shapes
        drawShapes();
        // get the screen matrix when all the transformations on the "paper" (fbo) or done.
        screenMatrix = ci::gl::getModelViewProjection();

    }
    {
        ci::gl::popMatrices();

		// draw background
		ci::gl::color(backgroundColor.r, backgroundColor.g, backgroundColor.b, 0.7);
		ci::gl::drawSolidRect(ci::Rectf(0, 0, 400, ci::app::getWindowHeight()));

    //    gl::setMatricesWindow(ci::app::getWindowSize());
        ci::gl::color(1, 1, 1);
        mFrameManager.drawLoop();

        if (useOverLay && mOverlayManager.isLive) {
            ci::gl::color(1, 0, 0, 0.5);
            float const radius = 10 + (sin(getElapsedSeconds() * 3) * 1);
            gl::drawSolidCircle(vec2(zoomCenterPoint.x + 14, 10 + 14), radius);
        }

		

		// draw poScene interface
        ci::gl::color(1, 1, 1);
        mScene->draw();
        drawCursor(getPressure(), lastPenPosition);
    }
}


void FramedApp::drawShapes() {

	if (!mTouchDown) return;

	switch (mCurrentToolState) {

	case ToolState::LINE:
		gl::drawLine(mShapeStartPoint, mShapeEndPoint);

		break;
	case ToolState::CIRCLE:
	{
		float radius = glm::distance(mShapeEndPoint, mShapeStartPoint);
		ci::gl::color(mTouchUI->getColor());
		gl::lineWidth(6);
		gl::drawStrokedCircle(mShapeStartPoint, radius);
		break;
	}
	case ToolState::RECTANGLE:
	{
		ci::gl::color(mTouchUI->getColor());
		gl::lineWidth(6);
		gl::drawStrokedRect(Rectf(mShapeStartPoint, mShapeEndPoint));
		break;
	}
	}
}

void FramedApp::drawCursor(float scale, vec2 position) const {

	gl::color(0.8, 0.8, 0.8);
	gl::lineWidth(2);
	float size = scale * 2;
	size = fminf(10, size);
	vec2 pVec2 = vec2(position.x, position.y);
	ci::gl::drawLine(pVec2 - ci::vec2(size, 0), pVec2 + ci::vec2(size, 0));
	ci::gl::drawLine(pVec2 + ci::vec2(0, -size), pVec2 + ci::vec2(0, +size));
}

void FramedApp::setNewFrameSize() {
    int nrFrames = GS()->nrOfFrames.value();
    mFrameManager.setup(nrFrames,frameSize);
    mTouchUI->setHeight(400 * frameSize.y / frameSize.x);
}

void FramedApp::drawDebug()
{


	mFps = getAverageFps();
    
	ImGui::Begin("Settings");
	ImGui::Text("framerate: %f", mFps);
    
	string pressureString = toString(mPenPressure);
	ImGui::LabelText("pen pressure", pressureString.c_str());
	ImGui::LabelText("ip", mNetworkManager->getIPadress().c_str());
	//    std::string localPoint = toString(localCoordinate.x);
	//    ImGui::LabelText("local", localPoint.c_str());

	ImGui::Dummy(ImVec2(0.0f, 20.0f));
	ImGui::Separator();
	ImGui::Dummy(ImVec2(0.0f, 20.0f));


	ImGui::Checkbox("show debug", &GS()->debugMode.value());
	if (ImGui::Checkbox("spout active", &GS()->isSpoutActive.value())) {
		setupSpout();
	}

	ImGui::Checkbox("projector mode", &GS()->projectorMode.value());
	if (ImGui::Checkbox("fullscreen", &GS()->isFullscreen.value())) {
		setFullScreen(GS()->isFullscreen.value());
	}

	ImGui::Dummy(ImVec2(0.0f, 20.0f));
	ImGui::Separator();
	ImGui::Dummy(ImVec2(0.0f, 20.0f));


	ImGui::Checkbox("disable pen on windows (restart needed)", &GS()->disablePenPressure.value());
	if (ImGui::Checkbox("hide clear button", &GS()->hideClearButton.value())) {
		mTouchUI->enableClearButton(!GS()->hideClearButton.value());
	}

	if (ImGui::Checkbox("hide save button", &GS()->hideSaveButton.value())) {
		mTouchUI->enableSaveButton(!GS()->hideSaveButton.value());
	}

	ImGui::Checkbox("overlay active", &useOverLay);

	if (useOverLay) {
		if (ImGui::TreeNode("overlay")) {

			mOverlayManager.drawGUI();

			ImGui::Dummy(ImVec2(0.0f, 20.0f));
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0.0f, 20.0f));

			ImGui::TreePop();
		}

	}

	ImGui::Dummy(ImVec2(0.0f, 20.0f));
	if (ImGui::TreeNode("Frame settings")) {
        ImGui::Dummy(ImVec2(0.0f, 20.0f));

		ImGui::SliderFloat("speed: ", &GS()->frameSpeed.value(), 2.f, 80.0f);
		if (ImGui::SliderInt("nr of frames", &GS()->nrOfFrames.value(), 1, 60)) {
			mFrameManager.changeNrOfFrames(GS()->nrOfFrames.value());
		}
		if (ImGui::Button("sync settings to all")) {
			mNetworkManager->setNrOfFrames(GS()->nrOfFrames.value());
			mNetworkManager->setFrameSpeed(GS()->frameSpeed.value());
		}
        ImGui::Dummy(ImVec2(0.0f, 20.0f));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, 20.0f));

        ImGui::SliderInt("frame width: ", &GS()->frameWidth.value(), 400, 6000);
        ImGui::SliderInt("frame height: ", &GS()->frameHeight.value(), 400, 6000);
        if (ImGui::Button("sync size to all (clears all)")) {
            frameSize.x = GS()->frameWidth.value();
            frameSize.y = GS()->frameHeight.value();
            mNetworkManager->setFrameSize(frameSize.x,frameSize.y);
            setNewFrameSize();
            
        }
        
        ImGui::TreePop();



        
	}

	ImGui::Dummy(ImVec2(0.0f, 20.0f));
	ImGui::Separator();
	ImGui::Dummy(ImVec2(0.0f, 20.0f));


	if (ImGui::SliderInt("group id", &GS()->groupId.value(), 1, 4)) {
		mNetworkManager->setGroupId(GS()->groupId.value());
	}

	ImGui::Dummy(ImVec2(0.0f, 20.0f));
	ImGui::Separator();
	ImGui::Dummy(ImVec2(0.0f, 20.0f));

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(0, 122, 30 + (sin(app::getElapsedSeconds() * 5) * 70))));
	if (ImGui::Button("save setttings", ImVec2(200, 40))) {
		GS()->mSettingManager.writeSettings();
	}
	ImGui::PopStyleColor();

	ImGui::End();
}

void prepareSettings(FramedApp::Settings* settings) {
	settings->setWindowSize(1600, 1200);

#ifdef CINDER_MAC
	settings->setHighDensityDisplayEnabled();
    //settings->setHighDensityDisplayEnabled(false);
#endif
}


CINDER_APP(FramedApp, RendererGl(RendererGl::Options().msaa(0)), prepareSettings);
