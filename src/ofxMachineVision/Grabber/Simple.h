#pragma once

#include "ofBaseTypes.h"
#include "ofTexture.h"

#include "Base.h"
#include "ofxMachineVision/Frame.h"

namespace ofxMachineVision {
	namespace Grabber {
		/**
		\brief Grabber::Simple is designed to give the same personality as ofVideoGrabber.
		You may be able to get improved performance by using Blocking or Callback Grabber instead
		*/
		class Simple : public Base, public ofBaseDraws, public ofBaseHasTexture, public ofBaseHasPixels, public ofBaseUpdates {
		public:
			Simple();
			~Simple();

			void open(int deviceID = 0);
			void close();
			void startCapture(const TriggerMode & = Trigger_Device, const TriggerSignalType & = TriggerSignal_Default);
			void stopCapture();
			void singleShot();

			bool isFrameNew() const { return this->currentFrameNew; }
			float getFps() const { return this->fps; }
			Microseconds getLastTimestamp() const { return this->lastTimestamp; }
			long getLastFrameIndex() const { return this->lastFrameIndex; }

			///Get a frame from the grabber and (attempt to) ensure that it was captured after the time of the function call
			/// Notes :
			///		* This function (by default) gives you a copy of the frame, so you don't need to worry about locking
			///		* If you want to ensure that this fresh frame is available from the other functions of this class (e.g. getPixels()), you should call update() after getFreshFrame()
			shared_ptr<Frame> getFreshFrame(bool giveCopy = true, float timeoutSeconds = 5.0f);

			/**
			 \name ofBaseUpdates
			 */
			//@{
			void update() override;
			//@}
			
			/**
			 \name ofBaseDraws
			 */
			//@{
			void draw(float x, float y) const override {
				this->draw(x, y, this->getWidth(), this->getHeight());
			}
			void draw(float x, float y, float w, float h) const override;
			void draw(const ofRectangle & rect) const override {
				draw(rect.x, rect.y, rect.width, rect.height);
			}
			/** \brief Get the width of the most recent captured frame */
			float getWidth() const override;
			/** \brief Get the height of the most recent captured frame */
			float getHeight() const override;
			//@}

		    /**
			 \name ofBaseHasTexture
			 */
			//@{
			ofTexture & getTexture() override;
			const ofTexture & getTexture() const override;
			void setUseTexture(bool useTexture) override;
			bool isUsingTexture() const override;
			//@}
        
			/**
			 \name ofBaseHasPixels
			 \brief Note that these getter functions are cached once per app frame. Use getFrame() for more direct access.
			 */
			//@{
			const ofPixels & getPixels() const override;
			ofPixels & getPixels() override;
			//@}

			/**
			\name Capture properties
			*/
			//@{
			void setExposure(Microseconds exposure) override;
			void setGain(float percent) override;
			void setFocus(float percent) override;
			void setSharpness(float percent) override;
			void setBinning(int binningX = 1, int binningY = 1) override;
			void setROI(const ofRectangle &) override;
			void setTriggerMode(const TriggerMode &, const TriggerSignalType &) override;
			void setGPOMode(const GPOMode &) override;
			//@}

			/**
			\name Frame functions

			Note our Frame policy:
			* frame is a shared_ptr inside Grabber::Simple
			* For each frame capture, a new shared_ptr<Frame> is instantiated (so you can keep old copies of frames elsewhere as you wish)
			* Where no valid frame is avalable, frame == Frame() (e.g. before first frame arrives).
			* getFrame() and setFrame() lock the pointer. You should not use frame = ... directly

			Other Grabbers can have different policies (e.g. you may choose to reuse the same Frame object for mulitple frame captures).
			*/
			//@{
			shared_ptr<Frame> getFrame() const;
			void setFrame(shared_ptr<Frame>);
			//@}
		protected:
			void callInRightThread(std::function<void()>);
			void notifyNewFrame(shared_ptr<Frame>);

			shared_ptr<Frame> frame;
			mutable ofMutex framePointerLock;

			ofPixels pixels; // every app frame we cache a set of pixels which we can use for simple access. if getPixels() returned from frame, then there would be locking issues.
			ofTexture texture;
			bool useTexture;

			bool newFrameWaiting;
			bool currentFrameNew;

			float fps;
			Microseconds lastTimestamp;
			long lastFrameIndex;
		};
	}
};