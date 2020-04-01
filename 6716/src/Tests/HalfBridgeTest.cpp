#include "../include/Tests/HalfBridgeTest.h"

Result HalfBridgeTest::test() const {
	device6716->configureVoltageReferanceSwitches(0x60);

	// 1
	device6716->invokeFunction(bu6716_setChannelConf, "bu6716_setChannelConf", device6716->channelsStateAsMask(), bu6716_MODE_HALF_BRIDGE, bu6716_GAIN_1, bu6716_COUPLING_DC, bu6716_INP_SRC_FP);
	device6716->invokeFunction(bu6716_setExcitationMonitor, "bu6716_setExcitationMonitor", bu6716_EXCMON_OFF);

	// 2
	deviceT028->invokeFunction(t028_setChannelsConfig, "t028_setChannelsConfig", device6716->channelsStateAsMask(), T028_MODE_HBRIDGE);

	// 3
	device6716->invokeFunction(bu6716_setPosExcitation, "bu6716_setPosExcitation", device6716->channelsStateAsMask(), 5.0);
	device6716->invokeFunction(bu6716_setNegExcitation, "bu6716_setNegExcitation", device6716->channelsStateAsMask(), 0.0);

	// 4
	auto values = device3416_6716->measureChannels(device6716->channelsStateAsMask());
	for (auto& channel : device6716->channels()) {
		if (channel.disabled())
			continue;
		channelsResults.at(channel.index()) = Result::VALUE::PASSED;
		if (!limit1201.checkValue(values[channel.index() - 1]))
			channelsResults.at(channel.index()) = Result::VALUE::FAILED;
		log(limit1201.lastStatusMsg());
	}

	// 5
	device6716->invokeFunction(bu6716_setMode, "bu6716_setMode", device6716->channelsStateAsMask(), bu6716_MODE_FULL_BRIDGE);

	// 6
	values = device3416_6716->measureChannels(device6716->channelsStateAsMask());
	for (auto& channel : device6716->channels()) {
		if (channel.disabled())
			continue;
		if (!limit1202.checkValue(values[channel.index() - 1]))
			channelsResults.at(channel.index()) = Result::VALUE::FAILED;
		log(limit1202.lastStatusMsg());
	}

	// 7
	device6716->invokeFunction(bu6716_setMode, "bu6716_setMode", device6716->channelsStateAsMask(), bu6716_MODE_HALF_BRIDGE);

	// 8
	device6716->invokeFunction(bu6716_setPosExcitation, "bu6716_setPosExcitation", device6716->channelsStateAsMask(), 10.0);
	device6716->invokeFunction(bu6716_setNegExcitation, "bu6716_setNegExcitation", device6716->channelsStateAsMask(), 0.0);

	// 9
	values = device3416_6716->measureChannels(device6716->channelsStateAsMask());
	for (auto& channel : device6716->channels()) {
		if (channel.disabled())
			continue;
		if (!limit1201.checkValue(values[channel.index() - 1]))
			channelsResults.at(channel.index()) = Result::VALUE::FAILED;
		log(limit1201.lastStatusMsg());
	}

	// 10 & 11 -> Removed

	return channelsResult();
}

HalfBridgeTest::HalfBridgeTest() : AbstractTest6716("Half Bridge") {}
