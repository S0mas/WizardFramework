#include "../include/Tests/ShuntTest.h"

void ShuntTest::quickAutobalance(ViUInt16 channelMask, ViInt16 adcGain) const {
	ViReal64 corr;
	device6716->setAutoDACPositive(-1.0);
	device6716->setAutoDACNegative(0.0);
	auto nvalues = device3416_6716->measureChannels(channelMask, adcGain);
	device6716->setAutoDACPositive(1.0);
	auto pvalues = device3416_6716->measureChannels(channelMask, adcGain);
	device6716->setAutoDACPositive(0.0);
	auto values = device3416_6716->measureChannels(channelMask, adcGain);
	for (auto& channel : device6716->channels()) {
		if (channel.disabled())
			continue;
		corr = -2.0 / (pvalues[channel.index()-1ll] - nvalues[channel.index() - 1ll]) * values[channel.index() - 1ll];
		device6716->setAutoDACPositive(corr);
		log(QString("Quick Auto Balance, ch_%1: %2").arg(channel.index()).arg(corr));
	}
}

Result ShuntTest::test() const {
	device6716->configureVoltageReferanceSwitches(0x60);

	// 1
	device6716->invokeFunction(bu6716_setChannelConf, "bu6716_setChannelConf", device6716->channelsStateAsMask(), bu6716_MODE_HALF_BRIDGE, bu6716_GAIN_100, bu6716_COUPLING_DC, bu6716_INP_SRC_FP);
	device6716->invokeFunction(bu6716_setExcitationMonitor, "bu6716_setExcitationMonitor", bu6716_EXCMON_OFF);

	// 2
	deviceT028->invokeFunction(t028_setChannelsConfig, "t028_setChannelsConfig", device6716->channelsStateAsMask(), T028_MODE_SHCAL);

	// 3
	device6716->invokeFunction(bu6716_setPosExcitation, "bu6716_setPosExcitation", device6716->channelsStateAsMask(), 10.0);
	device6716->invokeFunction(bu6716_setNegExcitation, "bu6716_setNegExcitation", device6716->channelsStateAsMask(), 0.0);
	bu3100_sleep(50);

	// 4
	quickAutobalance(device6716->channelsStateAsMask());
	bu3100_sleep(50);

	// 5
	auto values = device3416_6716->measureChannels(device6716->channelsStateAsMask());
	for (auto& channel : device6716->channels()) {
		if (channel.disabled())
			continue;
		channelsResults.at(channel.index()) = Result::VALUE::PASSED;
		if (!limit1701.checkValue(values[channel.index() - 1ll], bu6716_GAIN_100))
			channelsResults.at(channel.index()) = Result::VALUE::FAILED;
		log(limit1701.lastStatusMsg());
	}

	// 6 
	device6716->invokeFunction(bu6716_setShuntCal, "bu6716_setShuntCal", device6716->channelsStateAsMask(), true);
	bu3100_sleep(50);

	// 7
	values = device3416_6716->measureChannels(device6716->channelsStateAsMask());
	for (auto& channel : device6716->channels()) {
		if (channel.disabled())
			continue;
		if (!limit1702.checkValue(values[channel.index() - 1ll], bu6716_GAIN_100))
			channelsResults.at(channel.index()) = Result::VALUE::FAILED;
		log(limit1702.lastStatusMsg());
	}

	return channelsResult();
}

ShuntTest::ShuntTest() : AbstractTest6716("Shunt") {}
