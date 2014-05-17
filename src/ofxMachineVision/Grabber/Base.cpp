#include "Base.h"

namespace ofxMachineVision {
	namespace Grabber {
		//----------
		Base::Base(DevicePtr device) {
			this->deviceType = getType(device);
			switch(this->deviceType) {
			case Device::Type_Blocking: {
				this->thread = shared_ptr<Utils::ActionQueueThread>(new Utils::ActionQueueThread());
				break;
			}
			case Device::Type_NotImplemented:
				OFXMV_FATAL << "Device not implemented";
				break;
			}
			this->baseDevice = device;
			this->deviceState = State_Closed;
		}

		//----------
		Base::~Base() {
			this->deviceState = State_Deleting;
			switch(this->deviceType) {
			case Device::Type_Blocking:
				this->thread->stopThread();
				this->thread->waitForThread();
			}
		}
	}
}