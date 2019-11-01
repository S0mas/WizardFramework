#include "../include/Tests/InitialExcitationTest.h"
#include "../include/defines.h"

bool InitialExcitationTest::test() const {
	ViReal64 offPos, offNeg, gPos, gNeg;
	int errorDetected = 0;
	connection->callAndThrowOnError6716(bu6716_reloadConfig, "bu6716_reloadConfig", true);
	connection->callAndThrowOnErrorT028(t028_setChannelsConfig, "t028_setChannelsConfig", CHANNEL_MASK, T028_MODE_EXCAL);
	for (int i = 0; i < bu6716_NUM_CHAN; i++) {
		if (!(CHANNEL_MASK & (1 << i)))
			continue;
		if (!excitCalibration(i + 1, &offPos, &offNeg, &gPos, &gNeg, false)) {
			printf("Excitation calibration failed\n");
			errorDetected |= 1 << i;
		}
		if (errorDetected == 0)
			printf("CH_%d: offsetPos=%.9lf, offsetNeg=%.9lf, gainPos=%lf, gainNeg=%lf\n", i + 1, offPos, offNeg, gPos, gNeg);
	}
	return errorDetected == 0;
}

InitialExcitationTest::InitialExcitationTest(const std::shared_ptr<Communication_6716>& connection) : Abstract6716Test("Initial Excitation", connection) {}
