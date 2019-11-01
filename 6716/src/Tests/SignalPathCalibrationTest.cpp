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
		printf("  CHANNEL %d...\n", i + 1);
		if (!signalPathCalibration(i + 1, offsets, gainCoeffs, storeToEEPROM)) {
			printf("Signal path calibration failed\n");
			errorDetected |= (1 << i);
		}
		if ((errorDetected & (1 << i)) == 0) {
			for (int g = 0; g < 4; g++)
				printf("CH_%d_GAIN_%d: offset=%.9lf, gainCoeff=%lf\n", i + 1, gain[g], offsets[g], gainCoeffs[g]);
		}
		else
			printf("CH_%d: ERROR\n", i + 1);
	}
	return errorDetected == 0;
}

SignalPathCalibrationTest::SignalPathCalibrationTest(const std::shared_ptr<Communication_6716>& connection, const bool storeToEEPROM) : Abstract6716Test("Signal Path Calibration", connection), storeToEEPROM(storeToEEPROM) {}