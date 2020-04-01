#include "../include/Tests/ChannelGainTest.h"

Result ChannelGainTest::test() const {
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

		const std::vector<Limit> limits = { limit1901, limit1902, limit1903, limit1904, limit1905, limit1906, limit1907, limit1908 };
		const std::vector<ViInt16> gains = { bu6716_GAIN_1, bu6716_GAIN_10, bu6716_GAIN_100, bu6716_GAIN_1000 };
		auto status = true;
		for (int i = 0; i < gains.size(); ++i) {
			log(QString("GAIN %1").arg(gains[i]));
			// 1 & 10 & 17 & 24
			device6716->invokeFunction(bu6716_setGain, "bu6716_setGain", channel.mask(), gains[i]);

			// 4 & 11 & 18 & 25
			ViReal64 realPosVoltageValue = setAndGetVrefVoltage(9.0 / gains[i]);

			// 5 & 12 & 19 & 26
			auto valuePos = device3416_6716->measureChannel(channel.index());
			status &= limits[i].checkValue(valuePos);
			log(limits[i].lastStatusMsg());

			// 6 & 13 & 20 & 27
			ViReal64 realNegVoltageValue = setAndGetVrefVoltage(-9.0 / gains[i]);

			// 7 & 14 & 21 & 28
			auto valueNeg = device3416_6716->measureChannel(channel.index());
			status &= limits[i].checkValueSigned(valueNeg);
			log(limits[i].lastStatusMsg());

			// 8 & 15 & 22 & 29
			status &= limits[i + 4].checkValue((realPosVoltageValue - realNegVoltageValue) / (valuePos - valueNeg) * gains[i] * gains[i]);
			log(limits[i + 4].lastStatusMsg());

			// 9 & 16 & 23 & 30
			device6100->invokeFunction(bu6100_setVoltRefOutput, "bu6100_setVoltRefOutput", 0.0);
		}
		device6716->invokeFunction(bu6716_setInputSrc, "bu6716_setInputSrc", channel.index(), bu6716_INP_SRC_FP);
		channelsResults.at(channel.index()) = status ? Result::VALUE::PASSED : Result::VALUE::FAILED;
		log(QString("Channel %1, status: %2").arg(channel.index()).arg(status ? "OK" : "Error"));
	}

	return channelsResult();
}

ChannelGainTest::ChannelGainTest() : AbstractTest6716("Channel Gain") {}
