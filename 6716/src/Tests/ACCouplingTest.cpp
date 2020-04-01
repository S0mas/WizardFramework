#include "../include/Tests/ACCouplingTest.h"

Result ACCouplingTest::test() const {
	device6716->configureVoltageReferanceSwitches(0x64);

	// 1
	device6716->invokeFunction(bu6716_setChannelConf, "bu6716_setChannelConf", device6716->channelsStateAsMask(), bu6716_MODE_FULL_BRIDGE, bu6716_GAIN_1, bu6716_COUPLING_DC, bu6716_INP_SRC_FP);
	device6716->invokeFunction(bu6716_setExcitationMonitor, "bu6716_setExcitationMonitor", bu6716_EXCMON_OFF);

	// 2
	device6716->invokeFunction(bu6716_setPosExcitation, "bu6716_setPosExcitation", device6716->channelsStateAsMask(), 0.0);
	device6716->invokeFunction(bu6716_setNegExcitation, "bu6716_setNegExcitation", device6716->channelsStateAsMask(), 0.0);
	device6716->setAutoDACNegative(0.0);
	device6716->setAutoDACPositive(0.0);

	for (auto& channel : device6716->channels()) {
		if (channel.disabled())
			continue;
		log(QString("CHANNEL %1").arg(channel.index()));
		// 3
		device6716->invokeFunction(bu6716_setInputSrc, "bu6716_setInputSrc", channel.index(), bu6716_INP_SRC_VREF);

		auto status = true;
		for (auto const& couplingAC : { true, false }) {
			// 4 & 10
			setAndGetVrefVoltage(couplingAC ? 9 : -9);

			// 5 & 11
			status &= couplingAC ? limit2003.checkValue(device3416_6716->measureChannel(channel.index())) : limit2003.checkValueSigned(device3416_6716->measureChannel(channel.index()));
			log(limit2003.lastStatusMsg());

			// 6 & 12
			ViUInt16 registerBitState = couplingAC ? 0x0200 : 0x0100;
			ViUInt16 registerState = device6716->readPortExpander(channel.index());
			device6716->writePortExpander(channel.index(), (registerState & 0xfcff) | registerBitState);

			// 7 & 13
			bu3100_sleep(1500);  // Discharge time - must be 1.5 sec

			// 8 & 14
			auto const limit = couplingAC ? limit2001 : limit2002;
			status &= limit.checkValue(device3416_6716->measureChannel(channel.index()));
			log(limit.lastStatusMsg());

			// 9 & 15
			device6716->invokeFunction(bu6716_setCoupling, "bu6716_setCoupling", channel.mask(), bu6716_COUPLING_DC);

			// 16
			device6100->invokeFunction(bu6100_setVoltRefOutput, "bu6100_setVoltRefOutput", 0.0);
		}
		device6716->invokeFunction(bu6716_setInputSrc, "bu6716_setInputSrc", channel.index(), bu6716_INP_SRC_FP);
		channelsResults.at(channel.index()) = status ? Result::VALUE::PASSED : Result::VALUE::FAILED;
		log(QString("Channel %1, status: %2").arg(channel.index()).arg(status ? "OK" : "Error"));
	}

	return channelsResult();
}

ACCouplingTest::ACCouplingTest() : AbstractTest6716("AC Coupling") {}
