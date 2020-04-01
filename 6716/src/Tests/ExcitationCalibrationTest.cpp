#include "../include/Tests/ExcitationCalibrationTest.h"

// This test is concatenation of two tests - Initial Excitation Calibration(nr 11) and Excitation Calibration(nr 22) - due to almost no difference in these two.
// <step nr> from Initial Excitation Calibration(are not marked) and Excitation Calibration(are marked with 'x')
bool ExcitationCalibrationTest::excitCalibration(ViInt16 channelIndex) const {
	ViUInt16 channelMask = (1 << (channelIndex - 1));
	log(QString("CHANNEL %1").arg(channelIndex));

	device6716->invokeFunction(bu6716_setMode, "bu6716_setMode", channelMask, bu6716_MODE_DI);

	// 2 & 2x
	device6716->invokeFunction(bu6716_setPosExcitation, "bu6716_setPosExcitation", channelMask, 0.0);
	device6716->invokeFunction(bu6716_setNegExcitation, "bu6716_setNegExcitation", channelMask, 0.0);

	// 3 & 3x
	device6716->invokeFunction(bu6716_setExcitationMonitor, "bu6716_setExcitationMonitor", bu6716_EXCMON_POSITIVE);

	ExcitationCalibrationCoefficient channelCalibrationCoefficient;
	// 4 & 4x
	channelCalibrationCoefficient.positiveOffset = -device3416_6716->measureChannel(channelIndex);
	device6716->invokeFunction(bu6716_setExcCalibCoeff, "bu6716_setExcCalibCoeff", channelMask, channelCalibrationCoefficient.positiveOffset, 0, 1, 1);

	// 4
	auto status = limit1101.checkValue(channelCalibrationCoefficient.positiveOffset);
	log(limit1101.lastStatusMsg());

	// 5 & 5x
	const double positiveExcitation = 9.0;
	device6716->invokeFunction(bu6716_setPosExcitation, "bu6716_setPosExcitation", channelMask, positiveExcitation);

	// 6 & 6x
	auto measured = device3416_6716->measureChannel(channelIndex);
	channelCalibrationCoefficient.positiveGain = positiveExcitation / measured;
	device6716->invokeFunction(bu6716_setExcCalibCoeff, "bu6716_setExcCalibCoeff", channelMask, channelCalibrationCoefficient.positiveOffset, 0, channelCalibrationCoefficient.positiveGain, 1);

	// 6
	status &= limit1102.checkValue(measured);
	log(limit1102.lastStatusMsg());

	// 7 & 7x
	device6716->invokeFunction(bu6716_setPosExcitation, "bu6716_setPosExcitation", channelMask, 0.0);
	device6716->invokeFunction(bu6716_setNegExcitation, "bu6716_setNegExcitation", channelMask, 0.0);

	// 8 & 8x
	device6716->invokeFunction(bu6716_setExcitationMonitor, "bu6716_setExcitationMonitor", bu6716_EXCMON_NEGATIVE);

	// 9 & 9x
	channelCalibrationCoefficient.negativeOffset = -device3416_6716->measureChannel(channelIndex);
	device6716->invokeFunction(bu6716_setExcCalibCoeff, "bu6716_setExcCalibCoeff", channelMask, channelCalibrationCoefficient.positiveOffset, channelCalibrationCoefficient.negativeOffset, channelCalibrationCoefficient.positiveGain, 1);

	// 9
	status &= limit1101.checkValue(channelCalibrationCoefficient.negativeOffset);
	log(limit1101.lastStatusMsg());

	// 10 & 10x
	const double negativeExcitation = -9.0;
	device6716->invokeFunction(bu6716_setNegExcitation, "bu6716_setNegExcitation", channelMask, negativeExcitation);

	// 11 & 11x
	measured = device3416_6716->measureChannel(channelIndex);
	channelCalibrationCoefficient.negativeGain = negativeExcitation / measured;
	device6716->invokeFunction(bu6716_setExcCalibCoeff, "bu6716_setExcCalibCoeff", channelMask, channelCalibrationCoefficient.positiveOffset, channelCalibrationCoefficient.negativeOffset, channelCalibrationCoefficient.positiveGain, channelCalibrationCoefficient.negativeGain);

	// 11
	status &= limit1103.checkValue(measured);
	log(limit1103.lastStatusMsg());

	// 12 & 12x
	device6716->invokeFunction(bu6716_setPosExcitation, "bu6716_setPosExcitation", channelMask, 0.0);
	device6716->invokeFunction(bu6716_setNegExcitation, "bu6716_setNegExcitation", channelMask, 0.0);

	// 12x
	status &= limit2201.checkValue(device3416_6716->measureChannel(channelIndex));
	log(limit2201.lastStatusMsg());

	// 13x
	device6716->invokeFunction(bu6716_setPosExcitation, "bu6716_setPosExcitation", channelMask, 10);
	device6716->invokeFunction(bu6716_setExcitationMonitor, "bu6716_setExcitationMonitor", bu6716_EXCMON_POSITIVE);
	status &= limit2202.checkValue(device3416_6716->measureChannel(channelIndex));
	log(limit2202.lastStatusMsg());

	// 14x
	device6716->invokeFunction(bu6716_setNegExcitation, "bu6716_setNegExcitation", channelMask, -10);
	device6716->invokeFunction(bu6716_setExcitationMonitor, "bu6716_setExcitationMonitor", bu6716_EXCMON_NEGATIVE);
	status &= limit2203.checkValue(device3416_6716->measureChannel(channelIndex));
	log(limit2203.lastStatusMsg());

	tempExcitationCalibrationData_.push_back(channelCalibrationCoefficient);
	device6716->invokeFunction(bu6716_setExcCalibCoeff, "bu6716_setExcCalibCoeff", channelMask, channelCalibrationCoefficient.positiveOffset, channelCalibrationCoefficient.negativeOffset, channelCalibrationCoefficient.positiveGain, channelCalibrationCoefficient.negativeGain);
	
	if (status)
		log(channelCalibrationCoefficient.toString());
	log(QString("Channel %1, status: %2").arg(channelIndex).arg(status ? "OK" : "Error"));

	return status;
}

// This test is concatenation of two tests - Initial Excitation Calibration(nr 11) and Excitation Calibration(nr 22) - due to almost no difference in these two.
// <step nr> from Initial Excitation Calibration(are not marked) and Excitation Calibration(are marked with 'x')
Result ExcitationCalibrationTest::test() const {
	//Clear old test software calib data
	tempExcitationCalibrationData_.clear();
	needToReloadExcitationCalibDataFromTestApplication = false;

	//Reset coefficient to 0 and gain to 1 (neutral values)
	device6716->invokeFunction(bu6716_resetExcCalibCoeff, "bu6716_resetExcCalibCoeff", device6716->channelsStateAsMask(), false);

	// 1 & 1x
	deviceT028->invokeFunction(t028_setChannelsConfig, "t028_setChannelsConfig", device6716->channelsStateAsMask(), T028_MODE_EXCAL);

	for (auto& channel : device6716->channels()) {
		if (channel.enabled())
			channelsResults.at(channel.index()) = excitCalibration(channel.index()) ? Result::VALUE::PASSED : Result::VALUE::FAILED;
	}

	if (channelsResult() != Result::VALUE::FAILED) {
		if (sender_->waitForAcceptOrDecline("Should replace the device's old calibration coefficients?")) {
			log("Saving new calibration data on the eeprom..");
			device6716->invokeFunction(bu6716_storeExcCalibCoeff, "bu6716_storeExcCalibCoeff", device6716->channelsStateAsMask());
			log("Calibration data saved!");
		}
		else
			needToReloadExcitationCalibDataFromTestApplication = true;
	}
	return channelsResult();
}

ExcitationCalibrationTest::ExcitationCalibrationTest() : AbstractTest6716("Excitation Calibration") {}
