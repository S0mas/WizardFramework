#include "../include/Tests/SignalPathCalibrationTest.h"

bool SignalPathCalibrationTest::signalPathCalibration(ViInt16 channelIndex) const {
	int channelMask = (1 << (channelIndex - 1));
	log(QString("CHANNEL %1").arg(channelIndex));

	// 2
	device6716->invokeFunction(bu6716_setPosExcitation, "bu6716_setPosExcitation", channelMask, 0.0);
	device6716->invokeFunction(bu6716_setNegExcitation, "bu6716_setNegExcitation", channelMask, 0.0);
	device6716->setAutoDACNegative(0.0);
	device6716->setAutoDACPositive(0.0);

	// 3
	device6716->invokeFunction(bu6716_setInputSrc, "bu6716_setInputSrc", channelIndex, bu6716_INP_SRC_VREF);

	auto status = true;
	const std::vector<int> gains = { 1, 10, 100, 1000 };
	std::vector<Limit> limits = { limit2301, limit2302, limit2303, limit2304, limit2305, limit2306, limit2307, limit2308 };
	for (int i = 0; i < gains.size(); ++i) {
		log(QString("GAIN = %1").arg(gains[i]));

		// 1 & 12 & 21 & 30
		device6716->invokeFunction(bu6716_setGain, "bu6716_setGain", channelMask, gains[i]);

		// 4 & 13 & 22 & 31
		device6100->invokeFunction(bu6100_setVoltRefOutput, "bu6100_setVoltRefOutput", 0);

		// 5 & 14 & 23 & 32
		auto valueOffset = device3416_6716->measureChannel(channelIndex);
		status &= limits[i].checkValue(valueOffset);
		log(limits[i].lastStatusMsg());

		// 6 & 15 & 24 & 33
		auto vrefVal = 9.0 / gains[i];
		auto realPosVoltageValue = setAndGetVrefVoltage(vrefVal);

		// 7 & 16 & 25 & 34
		auto valuePos = device3416_6716->measureChannel(channelIndex) - valueOffset;
		status &= limits[i + 4].checkValue(valuePos);
		log(limits[i + 4].lastStatusMsg());

		// 8 & 17 & 26 & 35
		auto realNegVoltageValue = setAndGetVrefVoltage(-vrefVal);

		// 9 & 18 & 27 & 36
		auto valueNeg = device3416_6716->measureChannel(channelIndex) - valueOffset;
		status &= limits[i + 4].checkValueSigned(valueNeg);
		log(limits[i + 4].lastStatusMsg());

		// 10 & 19 & 28 & 37
		SignalPathCalibrationCoefficient channelSignalPathCalibData;
		channelSignalPathCalibData.offsets[i] = -valueOffset / gains[i];
		channelSignalPathCalibData.gainCoefficients[i] = (realPosVoltageValue - realNegVoltageValue) / (valuePos / gains[i] - valueNeg / gains[i]);

		// 11 & 20 & 29 & 38
		tempSignalPathCalibrationData_.push_back(channelSignalPathCalibData);
		if (status)
			device6716->invokeFunction(bu6716_setSignalPathCalibCoeff, "bu6716_setSignalPathCalibCoeff", channelMask, gains[i], channelSignalPathCalibData.offsets[i], channelSignalPathCalibData.gainCoefficients[i]);
	}

	// 39
	device6716->invokeFunction(bu6716_setGain, "bu6716_setGain", channelMask, 1);
	device6100->invokeFunction(bu6100_setVoltRefOutput, "bu6100_setVoltRefOutput", 0);

	device6716->invokeFunction(bu6716_setInputSrc, "bu6716_setInputSrc", channelMask, bu6716_INP_SRC_FP);
	log(QString("Channel %1, status: %2").arg(channelIndex).arg(status ? "OK" : "Error"));
	return status;
}

Result SignalPathCalibrationTest::test() const {
	device6716->configureVoltageReferanceSwitches(0x64);

	//Clear old test software calib data
	tempSignalPathCalibrationData_.clear();
	needToReloadSignalPathCalibDataFromTestApplication = false;

	//Reset coefficient to 0 and gain to 1 (neutral values)
	device6716->invokeFunction(bu6716_resetSignalPathCalibCoeff, "bu6716_resetSignalPathCalibCoeff", device6716->channelsStateAsMask(), false);

	// 1
	deviceT028->invokeFunction(t028_setChannelsConfig, "t028_setChannelsConfig", device6716->channelsStateAsMask(), T028_MODE_EXCAL);
	device6716->invokeFunction(bu6716_setExcitationMonitor, "bu6716_setExcitationMonitor", bu6716_EXCMON_OFF);
	device6716->invokeFunction(bu6716_setChannelConf, "bu6716_setChannelConf", device6716->channelsStateAsMask(), bu6716_MODE_FULL_BRIDGE, bu6716_GAIN_1, bu6716_COUPLING_DC, bu6716_INP_SRC_FP);

	for (auto& channel : device6716->channels()) {
		if (channel.enabled())
			channelsResults.at(channel.index()) = signalPathCalibration(channel.index()) ? Result::VALUE::PASSED : Result::VALUE::FAILED; 
	}

	if (channelsResult() != Result::VALUE::FAILED) {
		if (sender_->waitForAcceptOrDecline("Should replace the device's old calibration coefficients?")) {
			log("Saving new calibration data on the eeprom..");
			device6716->invokeFunction(bu6716_storeSignalPathCalibCoeff, "bu6716_storeSignalPathCalibCoeff", device6716->channelsStateAsMask());
			log("Calibration data saved!");
		}
		else
			needToReloadSignalPathCalibDataFromTestApplication = true;
	}
	return channelsResult();
}

SignalPathCalibrationTest::SignalPathCalibrationTest() : AbstractTest6716("Signal Path Calibration") {}
