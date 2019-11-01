#include "../include/Tests/AutobalanceDACTest.h"
#include "../include/defines.h"

bool AutobalanceDACTest::test() const {
	const ViInt32 gain = bu6716_GAIN_100;
	connection->callAndThrowOnError6716(bu6716_reloadConfig, "bu6716_reloadConfig", true);
	// 1
	connection->callAndThrowOnError6716(bu6716_setChannelConf, "bu6716_setChannelConf", CHANNEL_MASK, bu6716_MODE_FULL_BRIDGE, gain, bu6716_COUPLING_DC, bu6716_INP_SRC_FP);
	connection->callAndThrowOnError6716(bu6716_setExcitationMonitor, "bu6716_setExcitationMonitor", bu6716_EXCMON_OFF);
	// 2
	setAutoDACPositive(CHANNEL_MASK, 0.0);
	setAutoDACNegative(CHANNEL_MASK, 0.0);
	// 3
	connection->callAndThrowOnError6716(bu6716_setVoltRefMode, "bu6716_setVoltRefMode", bu6716_VREF_MODE_INT_MONITOR_OFF);
	connection->callAndThrowOnError6716(bu6716_setVoltRefOutput, "bu6716_setVoltRefOutput", 0.0);
	int errorDetected = 0;
	for (int i = 0; i < bu3416_NUM_CHAN; i++) {
		if (!(CHANNEL_MASK & (1 << i)))
			continue;
		ViUInt16 channelMask = 1 << i;  // Only one channel at a time
		connection->callAndThrowOnError6716(bu6716_setInputSrc, "bu6716_setInputSrc", i + 1, bu6716_INP_SRC_VREF);
		// 4
		setAutoDACPositive(channelMask, 10.0);
		bu3100_sleep(50);

		auto const& checkValueLambda = [this, i, gain](const std::string& limitName, const ViReal64 expected) {
			return checkValue_oneChannel(i + 1, readValue_oneChannel(bu3416_GAIN_1, i + 1, 0.1), limitName, expected * gain, 50e-3 * gain, 50e-3 * gain);
		};

		// 5 
		errorDetected = checkValueLambda("L1601", 30e-3);

		// 6
		setAutoDACPositive(channelMask, -10.0);
		bu3100_sleep(50);

		// 7
		errorDetected |= checkValueLambda("L1602", -30e-3);

		// 8 & 9
		setAutoDACPositive(channelMask, 0.0);
		setAutoDACNegative(channelMask, 10.0);
		bu3100_sleep(50);

		// 10
		errorDetected |= checkValueLambda("L1602", -30e-3);

		// 11
		setAutoDACNegative(channelMask, -10.0);
		bu3100_sleep(50);

		// 12
		errorDetected |= checkValueLambda("L1601", 30e-3);

		// 13
		setAutoDACPositive(channelMask, 0.0);
		setAutoDACNegative(channelMask, 0.0);
		connection->callAndThrowOnError6716(bu6716_setInputSrc, "bu6716_setInputSrc", i + 1, bu6716_INP_SRC_FP);
	}
	return errorDetected == 0;
}

AutobalanceDACTest::AutobalanceDACTest(const std::shared_ptr<Communication_6716>& connection) : Abstract6716Test("AutoBalance DAC", connection) {}
