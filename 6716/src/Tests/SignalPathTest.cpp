#include "../include/Tests/SignalPathTest.h"

Result SignalPathTest::test() const {
	// 1
	device6716->invokeFunction(bu6716_setChannelConf, "bu6716_setChannelConf", device6716->channelsStateAsMask(), bu6716_MODE_FULL_BRIDGE, bu6716_GAIN_1, bu6716_COUPLING_DC, bu6716_INP_SRC_FP);
	device6716->invokeFunction(bu6716_setExcitationMonitor, "bu6716_setExcitationMonitor", bu6716_EXCMON_OFF);

	// 2
	device6716->invokeFunction(bu6716_setPosExcitation, "bu6716_setPosExcitation", device6716->channelsStateAsMask(), 0.0);
	device6716->invokeFunction(bu6716_setNegExcitation, "bu6716_setNegExcitation", device6716->channelsStateAsMask(), 0.0);
	device6716->setAutoDACNegative(0.0);
	device6716->setAutoDACPositive(0.0);

	// 3
	device6716->configureVoltageReferanceSwitches(0x74);

	// 4
	deviceT028->invokeFunction(t028_setChannelsConfig, "t028_setChannelsConfig", device6716->channelsStateAsMask(), T028_MODE_EXCAL);
	deviceT028->invokeFunction(t028_setRelay, "t028_setRelay", T028_CHAN_ALL, T028_RELAY_5, T028_ON);

	for (auto const voltage : {9.0, -9.0}) {
		// 5 & 7
		device6100->invokeFunction(bu6100_setVoltRefOutput, "bu6100_setVoltRefOutput", voltage);

		// 6 & 8
		auto values = device3416_6716->measureChannels(device6716->channelsStateAsMask());
		for (auto& channel : device6716->channels()) {
			if (channel.disabled())
				continue;
			auto status = voltage > 0 ? limit1801.checkValue(values[channel.index() - 1]) : limit1801.checkValueSigned(values[channel.index() - 1]);
			log(limit1801.lastStatusMsg());
			channelsResults.at(channel.index()) = status ? Result::VALUE::PASSED : Result::VALUE::FAILED;
		}
	}

	return channelsResult();
}

SignalPathTest::SignalPathTest() : AbstractTest6716("Signal Path") {}
