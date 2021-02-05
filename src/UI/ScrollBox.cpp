//
// Created by lab101 on 28/12/2020.
//

#include "ScrollBox.h"
#include "poScene/ShapeView.h"
#include "mathHelper.h"
#include "AssetLoader/AssetsCache.h"
#include "cinder/app/App.h"
#include "cinder/gl/scoped.h"

// dependency should be removed later
#include "../Helpers/GlobalSettings.h"

using namespace po::scene;
using namespace ci;

ScrollBoxRef ScrollBox::create()
{
	ScrollBoxRef ref = std::make_shared<ScrollBox>();
	return ref;
}

ScrollBox::ScrollBox() {

}

void ScrollBox::setup(float width, float height, float thumbScale) {


	mFbo = gl::Fbo::create(width, height);

	mThumbScale = thumbScale;
	mMaxThumbWidth = 150;


	  // touch events
	getSignal(MouseEvent::DOWN_INSIDE).connect([=](po::scene::MouseEvent& event) {
		mIsPressed = true;

		auto pos = event.getLocalPos();
        mLastPosition = pos.y;
        mStartPosition = mLastPosition;
		});

	getSignal(MouseEvent::DRAG_INSIDE).connect([=](po::scene::MouseEvent& event) {
		if (mIsPressed)
		{
			updateMove(event);
		}
		});



	getSignal(MouseEvent::UP).connect([=](po::scene::MouseEvent& event) {

		if(mIsPressed){
            mIsPressed = false;

            float newPosY = event.getLocalPos().y;
            float  diff  = fabsf(newPosY - mStartPosition);

            if(diff < 20){
                int  indexOffset = floor((newPosY / mScaledFrameSize.y)) - 1;
                currentIndex += indexOffset;
            }

            targetIndex = round(currentIndex);
            mOnValueChanged.emit(getIndex(targetIndex));
		}

	});


	mGlsl = gl::GlslProg::create(ci::app::loadAsset("passthrough.vert"), ci::app::loadAsset("color.frag"));

	setSize(mFbo->getSize());

	// update to fill the fbo for the initial color set.
	currentIndex = 0;
	targetIndex = 0;
	mIsPressed = false;
	mThumbScale = 0.1;
	mOffset = 0;

	update();
}


void ScrollBox::updateMove(po::scene::MouseEvent& event) {

	float diff = mLastPosition - event.getLocalPos().y;
    mLastPosition = event.getLocalPos().y;

	targetIndex += diff / mScaledFrameSize.y;
	currentIndex = targetIndex;
}


int ScrollBox::getIndex(int index) {

	while (index < 0) {
		index += mTextures.size();
	}

	return index % mTextures.size();
}



void ScrollBox::setTextures(std::vector<ci::gl::TextureRef> textures) {
	mTextures = textures;


	if (mTextures.size() > 0) {

        mRenderFbo = true;
		ci::vec2 frameSize = textures[0]->getSize();
		//mScaledFrameSize = frameSize * mThumbScale;
		mScaledFrameSize.x = mMaxThumbWidth;
		mScaledFrameSize.y = mMaxThumbWidth * frameSize.y / frameSize.x;
	}
}



void ScrollBox::update() {


	currentIndex += (targetIndex - currentIndex) * 0.001;

	if (mRenderFbo && mTextures.size() > 0)
	{
		mRenderFbo = false;
		gl::ScopedFramebuffer fbScp(mFbo);
		gl::ScopedViewport fbVP(mFbo->getSize());
		gl::setMatricesWindow(mFbo->getSize());
		//            ci::gl::GlslProgRef textureShader = ci::gl::getStockShader(ci::gl::ShaderDef().texture().color());
		//            gl::ScopedGlslProg glslProg(textureShader);
		gl::clear(ColorA(0, 0, 0, 0));


		float y = mScaledFrameSize.y - mOffset;


		ci::gl::color(1, 1, 1, 1);

		for (float i = -2; i <= 4; i++) {

			int index = getIndex(i + (int)targetIndex);
			float div = fmodf(targetIndex, 1.0f);

			ci::gl::pushMatrices();

			float yOffset = y + (((float)i) * mScaledFrameSize.y) + (-div * mScaledFrameSize.y);
			ci::gl::translate(2, yOffset);

			auto s = mTextures[index];
			auto drawFrame = Rectf(0, 0, mScaledFrameSize.x, mScaledFrameSize.y - 1);
			ci::gl::draw(s, drawFrame);
			if (index == 0) {
				gl::color(0, 1, 0);
				gl::drawLine(vec2(0, 0), vec2(mScaledFrameSize.x, 0));
			}
			ci::gl::color(1, 1, 1, 1);

			float height = GS()->mTextureFont->measureString("D").y;
			GS()->mTextureFont->drawString(std::to_string(index + 1), vec2(14, height + 28));

			ci::gl::popMatrices();

		}

		auto activeFrame = Rectf(0, mScaledFrameSize.y, mScaledFrameSize.x - 4, mScaledFrameSize.y * 2);
		activeFrame.offset(vec2(4, 0));
		ci::gl::drawStrokedRect(activeFrame, 4);

		gl::setMatricesWindow(ci::app::getWindowSize());
	}

}


void ScrollBox::draw() {

	View::draw();
	gl::draw(mFbo->getColorTexture());
}


void ScrollBox::setActiveFrame(int index) {
    currentIndex = index;
    targetIndex = index;
    mRenderFbo = true;
}
