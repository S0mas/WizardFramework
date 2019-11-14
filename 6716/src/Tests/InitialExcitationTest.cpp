#include "../include/Tests/InitialExcitationTest.h"
#include "../include/defines.h"

bool InitialExcitationTest::test() const {
	ViReal64 offPos, offNeg, gPos, gNeg;
	
	connection->callAndThrowOnError6716(bu6716_reloadConfig, "bu6716_reloadConfig", true);
	connection->callAndThrowOnErrorT028(t028_setChannelsConfig, "t028_setChannelsConfig", CHANNEL_MASK, T028_MODE_EXCAL);
	configureVoltageReferanceSwitches(0x60);

	for (int i = 0; i < bu6716_NUM_CHAN; i++) {
		if (!(CHANNEL_MASK & (1 << i)))
			continue;
		if (!excitCalibration(i + 1, &offPos, &offNeg, &gPos, &gNeg, getStoreCalibrationDataToEeprom())) {
			log("Excitation calibration failed");
			channelsErrorsMask |= 1 << i;
		}
		if (channelsErrorsMask == 0)
			log(QString("CH_%1: offsetPos=%2, offsetNeg=%3, gainPos=%4, gainNeg=%5").arg(i + 1).arg(offPos).arg(offNeg).arg(gPos).arg(gNeg));
	}
	return channelsErrorsMask == 0;
}

InitialExcitationTest::InitialExcitationTest(const std::shared_ptr<Communication_6716>& connection) : Abstract6716Test("Initial Excitation", connection, true) {}
