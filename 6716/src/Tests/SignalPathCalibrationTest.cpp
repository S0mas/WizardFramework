#include "../include/Tests/SignalPathCalibrationTest.h"
#include "../include/defines.h"

bool SignalPathCalibrationTest::test() const {
	ViReal64 gainCoeffs[4] = { 0 };
	ViReal64 offsets[4] = { 0 };
	int errorDetected = 0;
	const ViInt16 gain[4] = { 1, 10, 100, 1000 };
	connection->callAndThrowOnError6716(bu6716_reloadConfig, "bu6716_reloadConfig", true);
	connection->callAndThrowOnErrorT028(t028_setChannelsConfig, "t028_setChannelsConfig", CHANNEL_MASK, T028_MODE_EXCAL);
	for (int i = 0; i < bu6716_NUM_CHAN; i++) {
		if (!(CHANNEL_MASK & (1 << i)))
			continue;
		log(QString("  CHANNEL %1...\n").arg(i + 1));
		if (!signalPathCalibration(i + 1, offsets, gainCoeffs, getStoreCalibrationDataToEeprom())) {
			log("Signal path calibration failed\n");
			errorDetected |= (1 << i);
		}
		if ((errorDetected & (1 << i)) == 0) {
			for (int g = 0; g < 4; g++)
				log(QString("CH_%1_GAIN_%2: offset=%3, gainCoeff=%4\n").arg(i + 1).arg(gain[g]).arg(offsets[g]).arg(gainCoeffs[g]));
		}
		else
			log(QString("CH_%1: ERROR\n").arg(i + 1));
	}
	bu3100_sleep(250);
	return errorDetected == 0;
}

SignalPathCalibrationTest::SignalPathCalibrationTest(const std::shared_ptr<Communication_6716>& connection) : Abstract6716Test("Signal Path Calibration", connection) {}