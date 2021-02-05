//
// Created by lab101 on 28/12/2020.
//

#pragma once

#include <poScene/ImageView.h>
#include "poScene/View.h"
#include "cinder/Signals.h"
#include "cinder/gl/gl.h"

typedef std::shared_ptr<class ScrollBox> ScrollBoxRef;

class ScrollBox : public po::scene::View
{

	float mLastPosition;
	float mStartPosition;
	float mCurrentAbsolutePosition;
	float currentIndex;
	float targetIndex;


	ci::gl::GlslProgRef mGlsl;
	ci::gl::FboRef  mFbo;
	bool mRenderFbo = true;

	bool mIsPressed = false;
	float mThumbScale;
	ci::vec2 mScaledFrameSize;


	float mOffset;
	float mMaxThumbWidth;

protected:

	float mScrollBoxValue;

	std::vector<ci::gl::TextureRef> mTextures;
	int getIndex(int index);


public:

	ci::signals::Signal<void(int value)> mOnValueChanged;
	static ScrollBoxRef create();
	ScrollBox();
	void setup(float x, float y, float thumbScale = 1);
	void setTextures(std::vector<ci::gl::TextureRef> textures);
    void setActiveFrame(int index);

	void draw() override;
	void update() override;
	void updateMove(po::scene::MouseEvent& event);
};

