#include "TouchButton.h"
#include "AssetLoader\AssetsCache.h"

using namespace std::placeholders;
using namespace po;
using namespace po::scene;

TouchButtonRef TouchButton::create()
{
	TouchButtonRef ref(new TouchButton());
	ref->setup();
	return ref;
}

void TouchButton::activate()
{
	if (mState != State::INACTIVE) {
		return;
	}

	setState(mActiveState);
}

void TouchButton::deactivate()
{
	if (mState == State::INACTIVE) {
		return;
	}

	mActiveState = mState;
	setState(State::INACTIVE);
}

TouchButton::TouchButton()
	: mState(State::NORMAL)
	, mActiveState(State::NORMAL)
	, mEventId(-1)
	, mId(0)
	, mType(Type::NORMAL)
	, mPropagationEnabled(false)
	, mEventMaxMoveDist(-1)
{
	setSize(ci::vec2(0, 0));
}

void TouchButton::setup()
{
	mTitleTextView = po::scene::TextView::create();
	mBackgroundImageView = po::scene::ImageView::create();
	mImageView = po::scene::ImageView::create();

	addSubview(mBackgroundImageView);
	addSubview(mImageView);
	addSubview(mTitleTextView);

	// Add event listeners
	mConnections += getSignal(po::scene::MouseEvent::DOWN_INSIDE).connect(std::bind(&TouchButton::mouseDownInside, this, ::_1));
	mConnections += getSignal(po::scene::MouseEvent::DRAG).connect(std::bind(&TouchButton::mouseDrag, this, ::_1));
	mConnections += getSignal(po::scene::MouseEvent::UP).connect(std::bind(&TouchButton::mouseUp, this, ::_1));

	mConnections += getSignal(po::scene::TouchEvent::BEGAN_INSIDE).connect(std::bind(&TouchButton::touchBeganInside, this, ::_1));
	mConnections += getSignal(po::scene::TouchEvent::MOVED).connect(std::bind(&TouchButton::touchMoved, this, ::_1));
	mConnections += getSignal(po::scene::TouchEvent::ENDED).connect(std::bind(&TouchButton::touchEnded, this, ::_1));

	mImageScales[State::HIGHLIGHTED] = ci::vec2(0.5, 0.5);
	mImageScales[State::NORMAL] = ci::vec2(1, 1);


}

void TouchButton::layoutSubviews()
{
}

void TouchButton::setState(State state)
{
	// Set cur state
	mState = state;

	// Update text + images

	// Background image
	{
		// Tint, offset, scale
		setTintOffsetAndAlphaForState(mBackgroundImageView, mBackgroundImageTints, mBackgroundImageOffsets, mBackgroundImageAlphas, state);
		setScaleForState(mBackgroundImageView, mBackgroundImageScales, state);

		// Image
		ci::gl::TextureRef backgroundTexture;
		getItemForState<ci::gl::TextureRef>(backgroundTexture, mBackgroundImages, state);
		mBackgroundImageView->setTexture(backgroundTexture);
		mBackgroundImageView->setVisible(backgroundTexture != nullptr ? true : false);
	}

	// Image
	{
		// Tint, offset, scale
		setTintOffsetAndAlphaForState(mImageView, mImageTints, mImageOffsets, mImageAlphas, state);
		setScaleForState(mImageView, mImageScales, state);

		// Image
		ci::gl::TextureRef imageTexture = nullptr;
		getItemForState<ci::gl::TextureRef>(imageTexture, mImages, state);

		mImageView->setTexture(imageTexture);
		mImageView->setVisible(imageTexture != nullptr ? true : false);

		if (imageTexture) {
			mImageView->setAlignment(po::scene::Alignment::CENTER_CENTER);
			mImageView->setPosition(mImageView->getWidth() * 0.5, mImageView->getHeight() * 0.5);
		}
		//setDrawBounds(true);
	}

	// Title
	{
		// Tint + Offset
		setTintOffsetAndAlphaForState(mTitleTextView, mTitleTints, mTitleOffsets, mTitleAlphas, state);

		// Text
		std::string title = "";
		getItemForState<std::string>(title, mTitles, state);
		mTitleText.setText(title);

		ci::Font font = font.getDefault();
		getItemForState<ci::Font>(font, mTitleFonts, state);
		mTitleText.setFont(font);

		ci::vec2 size = mTitleText.getSize();
		getItemForState<ci::vec2>(size, mTitleSizes, state);
		mTitleText.setSize(size);

		mTitleTextView->setCiTextBox(mTitleText);
		mTitleTextView->setVisible(title != "" ? true : false);
	}

	//	Send signal state has been set
	mSignalStateSet.emit(std::dynamic_pointer_cast<TouchButton>(shared_from_this()));
}

void TouchButton::setTintOffsetAndAlphaForState(ViewRef view, std::map<State, ci::Color> tints, std::map<State, ci::vec2> offsets, std::map<State, float> alphas, State state)
{
	ci::Color color(1.f, 1.f, 1.f);
	getItemForState<ci::Color>(color, tints, state);
	view->setFillColor(color);

	ci::vec2 offset(0.f);
	getItemForState<ci::vec2>(offset, offsets, state);
	view->setPosition(offset);

	float alpha(1.f);
	getItemForState<float>(alpha, alphas, state);
	view->setAlpha(alpha);
}

void TouchButton::setScaleForState(ViewRef view, std::map<State, ci::vec2> scales, State state)
{
	ci::vec2 scale(1.f);
	getItemForState<ci::vec2>(scale, scales, state);
	view->setScale(scale);
}


// Image getters
ci::gl::TextureRef TouchButton::getBackgroundImage(State forState)
{
	if (mBackgroundImages.count(forState) == 0) {
		return nullptr;
	}

	return mBackgroundImages[forState];
}

ci::gl::TextureRef TouchButton::getImage(State forState)
{
	if (mImages.count(forState) == 0) {
		return nullptr;
	}

	return mImages[forState];
}

// Image, text, color and offset setters
void TouchButton::setBackgroundImage(ci::gl::TextureRef image, State state) { setItemForState<ci::gl::TextureRef>(image, mBackgroundImages, state); setSize(image->getSize()); }
void TouchButton::setBackgroundImageOffset(ci::vec2 offset, State state) { setItemForState<ci::vec2>(offset, mBackgroundImageOffsets, state); }
void TouchButton::setBackgroundImageScale(ci::vec2 scale, State state) { setItemForState<ci::vec2>(scale, mBackgroundImageScales, state); }
void TouchButton::setBackgroundImageTint(ci::Color color, State state) { setItemForState<ci::Color>(color, mBackgroundImageTints, state); }
void TouchButton::setBackgroundImageAlpha(float alpha, State state) { setItemForState<float>(alpha, mBackgroundImageAlphas, state); }

void TouchButton::setImage(ci::gl::TextureRef image, State state) { setItemForState<ci::gl::TextureRef>(image, mImages, state); setSize(image->getSize()); }
void TouchButton::setImageOffset(ci::vec2 offset, State state) { setItemForState<ci::vec2>(offset, mImageOffsets, state); }
void TouchButton::setImageScale(ci::vec2 scale, State state) { setItemForState<ci::vec2>(scale, mImageScales, state); }
void TouchButton::setImageTint(ci::Color color, State state) { setItemForState<ci::Color>(color, mImageTints, state); }
void TouchButton::setImageAlpha(float alpha, State state) { setItemForState<float>(alpha, mImageAlphas, state); }

void TouchButton::setTitle(std::string title, State state) { setItemForState < std::string >(title, mTitles, state); }
void TouchButton::setTitleFont(ci::Font font, State state) { setItemForState < ci::Font >(font, mTitleFonts, state); }

void TouchButton::setTitleOffset(ci::vec2 offset, State state) { setItemForState<ci::vec2>(offset, mTitleOffsets, state); }
void TouchButton::setTitleTint(ci::Color color, State state) { setItemForState<ci::Color>(color, mTitleTints, state); }
void TouchButton::setTitleAlpha(float alpha, State state) { setItemForState<float>(alpha, mTitleAlphas, state); }

void TouchButton::setTint(ci::Color color, State state)
{
	setBackgroundImageTint(color, state);
	setImageTint(color, state);
	setTitleTint(color, state);
}

void TouchButton::setTitleSize(ci::vec2 size, State forState)
{
	setItemForState<ci::vec2>(size, mTitleSizes, forState);
}

void TouchButton::setTitleWidth(float width, State forState)
{
	setTitleSize(mTitleText.getSize() + ci::ivec2(width, 0), forState);
}

// Event listeners
bool TouchButton::posIsWithinMaxMoveLimits(ci::vec2 pos)
{
	if (mEventMaxMoveDist.x >= 0 || mEventMaxMoveDist.y >= 0) {
		ci::vec2 moveDist = pos - mEventStartPos;

		if (mEventMaxMoveDist.x >= 0 && fabs(moveDist.x) > mEventMaxMoveDist.x) {
			return false;
		}
		else if (mEventMaxMoveDist.y >= 0 && fabs(moveDist.y) > mEventMaxMoveDist.y) {
			return false;
		}
	}

	return true;
}


void TouchButton::eventBeganInside(int id, ci::vec2 windowPos)
{
	if (mState == State::INACTIVE) {
		return;
	}

	if (mEventId == -1) {
		mEventId = id;
		mEventStartPos = windowToLocal(windowPos);
		mEventStartState = mState;
		setState(State::HIGHLIGHTED);
	}
}

void TouchButton::eventMoved(int id, ci::vec2 windowPos)
{
	if (mState == State::INACTIVE) {
		return;
	}

	if (mEventId == id) {
		// Check to see if we've gone past max move dist
		if (!posIsWithinMaxMoveLimits(windowToLocal(windowPos))) {
			mEventId = -1;
			setState(mEventStartState);
			return;
		}

		// If we're good, keep on showing
		if (pointInside(windowPos)) {
			setState(State::HIGHLIGHTED);
		}
		else {
			setState(State::NORMAL);
		}
	}
}

void TouchButton::eventEnded(int id, ci::vec2 windowPos)
{
	if (mState == State::INACTIVE) {
		return;
	}

	if (mEventId == id) {
		if (pointInside(windowPos)) {
			// Normal just send pressed
			if (mType == Type::NORMAL) {
				setState(State::NORMAL);
				mSignalPressed.emit(std::dynamic_pointer_cast<TouchButton>(shared_from_this()));
			}

			// Toggle switch State
			else {
				setState(mEventStartState == State::NORMAL ? State::SELECTED : State::NORMAL);
				mSignalToggled.emit(std::dynamic_pointer_cast<TouchButton>(shared_from_this()));
			}
		}

		mEventId = -1;
	}
}

void TouchButton::mouseDownInside(po::scene::MouseEvent& event)
{
	event.setPropagationEnabled(mPropagationEnabled);
	eventBeganInside(0, event.getWindowPos());
}

void TouchButton::mouseDrag(po::scene::MouseEvent& event)
{
	event.setPropagationEnabled(mPropagationEnabled);
	eventMoved(0, event.getWindowPos());
}

void TouchButton::mouseUp(po::scene::MouseEvent& event)
{
	event.setPropagationEnabled(mPropagationEnabled);
	eventEnded(0, event.getWindowPos());
}

void TouchButton::touchBeganInside(po::scene::TouchEvent& event)
{
	event.setPropagationEnabled(mPropagationEnabled);
	eventBeganInside(event.getId(), event.getWindowPos());
}

void TouchButton::touchMoved(po::scene::TouchEvent& event)
{
	event.setPropagationEnabled(mPropagationEnabled);
	eventMoved(event.getId(), event.getWindowPos());
}

void TouchButton::touchEnded(po::scene::TouchEvent& event)
{
	event.setPropagationEnabled(mPropagationEnabled);
	eventEnded(event.getId(), event.getWindowPos());
}


ci::Rectf TouchButton::getBounds()
{
	ci::Rectf  bounds = View::getBounds();

	if (mBackgroundImageView && mBackgroundImageView->getTexture() != nullptr) {
		float width = mBackgroundImageView->getScaledWidth();
		float height = mBackgroundImageView->getScaledHeight();
		bounds.set(0, 0, width, height);
	}

	bounds.inflate(mInflate);
	return bounds;
}

void TouchButton::setInflate(ci::vec2 inflate) {
	mInflate = inflate;
}
