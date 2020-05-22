#include "../../include/Tests/QuarterBridgeTest.h"

Result QuarterBridgeTest::test() const {
	device6716->configureVoltageReferanceSwitches(0x60);

	// 1
	device6716->invokeFunction(bu6716_setChannelConf, "bu6716_setChannelConf", device6716->channelsStateAsMask(), bu6716_MODE_QBRIDGE_120, bu6716_GAIN_1, bu6716_COUPLING_DC, bu6716_INP_SRC_FP);
	device6716->invokeFunction(bu6716_setExcitationMonitor, "bu6716_setExcitationMonitor", bu6716_EXCMON_OFF);

	// 2
	deviceT028->invokeFunction(t028_setChannelsConfig, "t028_setChannelsConfig", device6716->channelsStateAsMask(), T028_MODE_QBRIDGE);

	// 3
	device6716->invokeFunction(bu6716_setPosExcitation, "bu6716_setPosExcitation", device6716->channelsStateAsMask(), 5.0);
	device6716->invokeFunction(bu6716_setNegExcitation, "bu6716_setNegExcitation", device6716->channelsStateAsMask(), 0.0);
	bu3100_sleep(50);

	// 4
	auto values = device3416_6716->measureChannels(device6716->channelsStateAsMask());
	for (auto& channel : device6716->channels()) {
		if (channel.disabled())
			continue;
		channelsResults.at(channel.index()) = Result::VALUE::PASSED;
		if (!limit1301.checkValue(values[channel.index() - 1ll]))
			channelsResults.at(channel.index()) = Result::VALUE::FAILED;
		log(limit1301.lastStatusMsg());
	}

	// 5
	device6716->invokeFunction(bu6716_setMode, "bu6716_setMode", device6716->channelsStateAsMask(), bu6716_MODE_DI);
	bu3100_sleep(100);
	device6716->invokeFunction(bu6716_setMode, "bu6716_setMode", device6716->channelsStateAsMask(), bu6716_MODE_QBRIDGE_350);
	bu3100_sleep(50);

	// 6
	values = device3416_6716->measureChannels(device6716->channelsStateAsMask());
	for (auto& channel : device6716->channels()) {
		if (channel.disabled())
			continue;
		if (!limit1302.checkValue(values[channel.index() - 1ll]))
			channelsResults.at(channel.index()) = Result::VALUE::FAILED;
		log(limit1302.lastStatusMsg());
	}

	return channelsResult();
}

QuarterBridgeTest::QuarterBridgeTest() : AbstractTest6716("Quarter Bridge") {}
