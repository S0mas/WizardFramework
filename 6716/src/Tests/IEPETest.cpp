#include "../include/Tests/IEPETest.h"

Result IEPETest::test() const {
	device6716->configureVoltageReferanceSwitches(0x60);
	
	// 1
	device6716->invokeFunction(bu6716_setChannelConf, "bu6716_setChannelConf", device6716->channelsStateAsMask(), bu6716_MODE_DI, bu6716_GAIN_10, bu6716_COUPLING_DC, bu6716_INP_SRC_FP);
	device6716->invokeFunction(bu6716_setExcitationMonitor, "bu6716_setExcitationMonitor", bu6716_EXCMON_OFF);

	deviceT028->invokeFunction(t028_setChannelsConfig, "t028_setChannelsConfig", device6716->channelsStateAsMask(), T028_MODE_EXCAL);
	for (auto& channel : device6716->channels()) {
		if (channel.disabled())
			continue;
		log(QString("CHANNEL %1").arg(channel.index()));

		// 2
		deviceT028->invokeFunction(t028_setRelay, "t028_setRelay", channel.index(), T028_RELAY_4, T028_ON);
		deviceT028->invokeFunction(t028_setRelay, "t028_setRelay", channel.index(), T028_RELAY_6, T028_ON);

		// 3
		limit2101.checkValue(device3416_6716->measureChannel(channel.index(), bu3416_GAIN_10), bu3416_GAIN_10);
		device6716->invokeFunction(bu6716_setMode, "bu6716_setMode", channel.mask(), bu6716_MODE_IEPE);

		// 4
		log(QString("IEPE connected"));
		ViInt16 iepeStatus[bu6716_NUM_CHAN];
		device6716->invokeFunction(bu6716_getCurrentStatus, "bu6716_getCurrentStatus", 0xffff, iepeStatus, nullptr);
		ViUInt16 iepeStatus_mask = 0;
		for (int j = 0; j < device6716->channels().size(); j++)
			if (iepeStatus[j] == bu6716_IEPE_OK)
				iepeStatus_mask |= (1 << j);
		auto status = true;
		if (iepeStatus[channel.index() - 1] == bu6716_IEPE_OK)
			log("OK: Detected valid IEPE current on tested channel");
		else if (iepeStatus[channel.index() - 1] == bu6716_IEPE_NO_CURRENT) {
			log("ERROR: IEPE current not detected on tested channel");
			status = false;
		}
		if (iepeStatus_mask & ~(channel.mask())) {
			log(QString("ERROR: IEPE current detected on different channel/s (icp_st: 0x%1 step 4)").arg(iepeStatus_mask));
			status = false;
		}

		// 5
		device6716->invokeFunction(bu6716_setMode, "bu6716_setMode", channel.mask(), bu6716_MODE_IEPE);  // Sets AC-pos, DC-neg coupling

		// 6
		deviceT028->invokeFunction(t028_setRelay, "t028_setRelay", channel.index(), T028_RELAY_6, T028_OFF);

		// 7
		log(QString("IEPE disconnected"));
		device6716->invokeFunction(bu6716_getCurrentStatus, "bu6716_getCurrentStatus", 0xffff, iepeStatus, nullptr);
		iepeStatus_mask = 0;
		for (int j = 0; j < device6716->channels().size(); j++)
			if (iepeStatus[j] == bu6716_IEPE_OK)
				iepeStatus_mask |= (1 << j);
		if (iepeStatus[channel.index() - 1] == bu6716_IEPE_NO_CURRENT)
			log("OK: IEPE current not detected on tested channel");
		else if (iepeStatus[channel.index() - 1] == bu6716_IEPE_OK) {
			log("ERROR: IEPE current detected on tested channel");
			status = false;
		}
		if (iepeStatus_mask) {
			log(QString("ERROR: IEPE current detected on different channels (icp_st: 0x1 step 7)").arg(iepeStatus_mask));
			status = false;
		}
		
		device6716->invokeFunction(bu6716_setChannelConf, "bu6716_setChannelConf", channel.mask(), bu6716_MODE_DI, bu6716_GAIN_1, bu6716_COUPLING_DC, bu6716_INP_SRC_FP);
		deviceT028->invokeFunction(t028_setRelay, "t028_setRelay", channel.index(), T028_RELAY_4, T028_OFF);
		log(QString("Channel %1, status: %2").arg(channel.index()).arg(status ? "OK" : "Error"));
		channelsResults.at(channel.index()) = status ? Result::VALUE::PASSED : Result::VALUE::FAILED;
	}
	return channelsResult();
}

IEPETest::IEPETest() : AbstractTest6716("IEPE") {}
