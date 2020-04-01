#include "../include/Tests/AutobalanceDACTest.h"

Result AutobalanceDACTest::test() const {
	device6716->configureVoltageReferanceSwitches(0x64);
	// 1
	const ViInt32 gain = bu6716_GAIN_100;
	device6716->invokeFunction(bu6716_setChannelConf, "bu6716_setChannelConf", device6716->channelsStateAsMask(), bu6716_MODE_FULL_BRIDGE, gain, bu6716_COUPLING_DC, bu6716_INP_SRC_FP);
	device6716->invokeFunction(bu6716_setExcitationMonitor, "bu6716_setExcitationMonitor", bu6716_EXCMON_OFF);

	// 2
	device6716->setAutoDACPositive(0.0);
	device6716->setAutoDACNegative(0.0);

	// 3
	device6100->invokeFunction(bu6100_setVoltRefOutput, "bu6100_setVoltRefOutput", 0.0);
	
	for (auto& channel : device6716->channels()) {
		if (channel.disabled())
			continue;
		log(QString("CHANNEL %1").arg(channel.index()));

		// 1
		device6716->invokeFunction(bu6716_setInputSrc, "bu6716_setInputSrc", channel.index(), bu6716_INP_SRC_VREF);

		// 4
		device6716->setAutoDACPositive(10.0);

		// 5 
		bool status = limit1601.checkValue(device3416_6716->measureChannel(channel.index(), gain), gain);
		log(limit1601.lastStatusMsg());

		// 6
		device6716->setAutoDACPositive(-10.0);

		// 7
		status &= limit1602.checkValue(device3416_6716->measureChannel(channel.index(), gain), gain);
		log(limit1602.lastStatusMsg());

		// 8
		device6716->setAutoDACPositive(0.0);
		
		// 9
		device6716->setAutoDACNegative(10.0);

		// 10
		status &= limit1602.checkValue(device3416_6716->measureChannel(channel.index(), gain), gain);
		log(limit1602.lastStatusMsg());

		// 11
		device6716->setAutoDACNegative(-10.0);

		// 12
		status &= limit1601.checkValue(device3416_6716->measureChannel(channel.index(), gain), gain);
		log(limit1601.lastStatusMsg());

		// 13
		device6716->setAutoDACPositive(0.0);
		device6716->setAutoDACNegative(0.0);

		device6716->invokeFunction(bu6716_setInputSrc, "bu6716_setInputSrc", channel.index(), bu6716_INP_SRC_FP);
		channelsResults.at(channel.index()) = status ? Result::VALUE::PASSED : Result::VALUE::FAILED;
		log(QString("Channel %1, status: %2").arg(channel.index()).arg(status ? "OK" : "Error"));
	}
	return channelsResult();
}

AutobalanceDACTest::AutobalanceDACTest() : AbstractTest6716("AutoBalance DAC") {}
