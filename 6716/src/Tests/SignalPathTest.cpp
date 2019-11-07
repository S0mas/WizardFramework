#include "../include/Tests/SignalPathTest.h"
#include "../include/defines.h"

bool SignalPathTest::test() const {
	int errorDetected = 0;
	const ViReal64 volts[2] = { 9.0, -9.0 };
	connection->callAndThrowOnError6716(bu6716_reloadConfig, "bu6716_reloadConfig", true);
	// 1
	connection->callAndThrowOnError6716(bu6716_setChannelConf, "bu6716_setChannelConf", CHANNEL_MASK, bu6716_MODE_FULL_BRIDGE, bu6716_GAIN_1, bu6716_COUPLING_DC, bu6716_INP_SRC_FP);
	connection->callAndThrowOnError6716(bu6716_setExcitationMonitor, "bu6716_setExcitationMonitor", bu6716_EXCMON_OFF);
	// 2
	connection->callAndThrowOnError6716(bu6716_setPosExcitation, "bu6716_setPosExcitation", CHANNEL_MASK, 0.0);
	connection->callAndThrowOnError6716(bu6716_setNegExcitation, "bu6716_setNegExcitation", CHANNEL_MASK, 0.0);

	auto autoDacNeg = getAutoDACNegative(CHANNEL_MASK);
	auto autoDacPos = getAutoDACPositive(CHANNEL_MASK);
	setAutoDACNegative(CHANNEL_MASK, 0.0);
	setAutoDACPositive(CHANNEL_MASK, 0.0);
	bu3100_sleep(50);
	// 4
	connection->callAndThrowOnErrorT028(t028_setChannelsConfig, "t028_setChannelsConfig", CHANNEL_MASK, T028_MODE_EXCAL);
	connection->callAndThrowOnErrorT028(t028_setRelay, "t028_setRelay", T028_CHAN_ALL, T028_RELAY_5, T028_ON);
	for (int j = 0; j < 2; j++) {
		// 5 & 7
		configureVoltageReferanceSwitches(0x74);
		connection->callAndThrowOnError6100(bu6100_setVoltRefOutput, "bu6100_setVoltRefOutput", volts[j]);
		// 6 & 8
		errorDetected |= checkValues(CHANNEL_MASK, readValues(bu3416_GAIN_1, CHANNEL_MASK, 0.1), j == 0 ? "L1801" : "L1802", volts[j], 10e-3, 10e-3);
	}
	// Restore
	connection->callAndThrowOnError6100(bu6100_setVoltRefOutput, "bu6100_setVoltRefOutput", 0.0);
	connection->callAndThrowOnErrorT028(t028_setRelay, "t028_setRelay", T028_CHAN_ALL, T028_RELAY_5, T028_OFF);

	for (int i = 0, n = 0; i < bu6716_NUM_CHAN; i++) {
		if (CHANNEL_MASK & (1 << i)) {
			setAutoDACNegative(CHANNEL_MASK, autoDacNeg[n]);
			setAutoDACPositive(CHANNEL_MASK, autoDacPos[n]);
			n++;
		}
	}
	bu3100_sleep(250);
	return errorDetected == 0;
}

SignalPathTest::SignalPathTest(const std::shared_ptr<Communication_6716>& connection) : Abstract6716Test("Signal Path", connection) {}
