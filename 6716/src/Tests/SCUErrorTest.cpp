#include "../../include/Tests/SCUErrorTest.h"

Result SCUErrorTest::test() const {
	device6716->configureVoltageReferanceSwitches(0x60);
	device6716->invokeFunction(bu6716_setChannelConf, "bu6716_setChannelConf", device6716->channelsStateAsMask(), bu6716_MODE_DI, bu6716_GAIN_1, bu6716_COUPLING_DC, bu6716_INP_SRC_FP);
	device6716->invokeFunction(bu6716_setExcitationMonitor, "bu6716_setExcitationMonitor", bu6716_EXCMON_OFF);
	deviceT028->invokeFunction(t028_setChannelsConfig, "t028_setChannelsConfig", device6716->channelsStateAsMask(), T028_MODE_EXCAL);
	for (auto& channel : device6716->channels()) {
		if (channel.disabled())
			continue;
		ViInt16 sts, stsAll[bu6716_NUM_CHAN];
		log(QString("Channel %1").arg(channel.index()));
		auto status = true;
		for (auto const scuErrorEnabled : {false, true}) {
			// Enable IEPE on T028 and on 6716 - the IEPE current shall be detected
			deviceT028->invokeFunction(t028_setChannelsConfig, "t028_setChannelsConfig", channel.mask(), T028_MODE_ICP);
			device6716->invokeFunction(bu6716_setMode, "bu6716_setMode", channel.mask(), bu6716_MODE_IEPE);
			device6716->invokeFunction(bu6716_getCurrentStatus, "bu6716_getCurrentStatus", channel.mask(), &sts, nullptr);
			if (sts != bu6716_IEPE_OK) {
				log("IEPE current NOT detected, but should be");
				status = false;
			}
			// Disable IEPE on T028, so the error should happen. Enable IEPE to SCU_ERR in iteration s==1.
			device6716->invokeFunction(bu6716_setIEPEScuErrorMask, "bu6716_setIEPEScuErrorMask", 0xffff, scuErrorEnabled);
			deviceT028->invokeFunction(t028_setChannelsConfig, "t028_setChannelsConfig", device6716->channelsStateAsMask(), T028_MODE_EXCAL);
			device6716->invokeFunction(bu6716_getCurrentStatus, "bu6716_getCurrentStatus", channel.mask(), &sts, nullptr);
			if (sts == bu6716_IEPE_OK) {
				log("IEPE current detected, but shouldn't be");
				status = false;
			}
			auto dsr = device6716->readDSR();
			if (!scuErrorEnabled) {
				if (dsr & 0x08) {
					log("SCU_ERR detected, but shouldn't be in iteration s==0");
					status = false;
				}
			}
			else {
				if (!(dsr & 0x08)) {
					log("SCU_ERR not detected, but should be in iteration s==1");
					status = false;
				}
			}
			device6716->invokeFunction(bu6716_getLatchedStatus, "bu6716_getLatchedStatus", stsAll, nullptr);
			for (int j = 0; j < bu6716_NUM_CHAN; j++) {
				if ((channel.index() - 1) == j) {
					if (stsAll[j] != 1) {
						log("No IEPE error detected, but should be");
						status = false;
					}
				}
				else {
					if (stsAll[j] != 0) {
						log("IEPE error detected, but shouldn't be");
						status = false;
					}
				}
			}
			device6716->invokeFunction(bu6716_getLatchedStatus, "bu6716_getLatchedStatus", stsAll, nullptr);
			for (int j = 0; j < bu6716_NUM_CHAN; j++) {
				if (stsAll[j] != 0) {
					log("IEPE error detected, but shouldn't be");
					status = false;
				}
			}
			device6716->invokeFunction(bu6716_setMode, "bu6716_setMode", channel.mask(), bu6716_MODE_DI);
			device6716->invokeFunction(bu6716_setIEPEScuErrorMask, "bu6716_setIEPEScuErrorMask", 0xffff, false);
		}
		channelsResults.at(channel.index()) = status ? Result::VALUE::PASSED : Result::VALUE::FAILED;
		log(QString("Channel %1, status: %2").arg(channel.index()).arg(status ? "OK" : "Error"));
	}
	return channelsResult();
}

SCUErrorTest::SCUErrorTest() : AbstractTest6716("SCU Error") {}
