#include "../include/Tests/ShuntTest.h"
#include "../include/defines.h"

ViStatus ShuntTest::quickAutobalance(ViUInt16 channelMask, ViInt16 adcGain) const noexcept {
	ViReal64 corr;
	setAutoDACPositive(channelMask, -1.0);
	setAutoDACNegative(channelMask, 0.0);
	bu3100_sleep(100);
	auto nvalues = readValues(adcGain, channelMask, 0.1);
	setAutoDACPositive(channelMask, 1.0);
	bu3100_sleep(100);
	auto pvalues = readValues(adcGain, channelMask, 0.1);
	setAutoDACPositive(channelMask, 0.0);
	bu3100_sleep(100);
	auto values = readValues(adcGain, channelMask, 0.1);
	for (int i = 0; i < bu6716_NUM_CHAN; i++) {
		if (channelMask & (1 << i)) {
			ViUInt16 mask = (1 << i);
			corr = -2.0 / (pvalues[i] - nvalues[i]) * values[i];
			setAutoDACPositive(mask, corr);
			printf("Quick Auto Balance, ch_%d: %lf\n", i + 1, corr);
		}
	}
	return VI_SUCCESS;
}
//OK
bool ShuntTest::test() const {
	const ViInt16 gain = bu6716_GAIN_100;
	connection->callAndThrowOnError6716(bu6716_reloadConfig, "bu6716_reloadConfig", true);
	// 1
	connection->callAndThrowOnError6716(bu6716_setChannelConf, "bu6716_setChannelConf", CHANNEL_MASK, bu6716_MODE_HALF_BRIDGE, gain, bu6716_COUPLING_DC, bu6716_INP_SRC_FP);
	connection->callAndThrowOnError6716(bu6716_setExcitationMonitor, "bu6716_setExcitationMonitor", bu6716_EXCMON_OFF);
	// 2
	connection->callAndThrowOnErrorT028(t028_setChannelsConfig, "t028_setChannelsConfig", CHANNEL_MASK, T028_MODE_SHCAL);
	// 3
	connection->callAndThrowOnError6716(bu6716_setPosExcitation, "bu6716_setPosExcitation", CHANNEL_MASK, 10.0);
	connection->callAndThrowOnError6716(bu6716_setNegExcitation, "bu6716_setNegExcitation", CHANNEL_MASK, 0.0);
	bu3100_sleep(50);
	// 4
	quickAutobalance(CHANNEL_MASK, bu3416_GAIN_1);
	bu3100_sleep(50);
	// 5
	auto errorDetected = checkValues(CHANNEL_MASK, readValues(bu3416_GAIN_1, CHANNEL_MASK, 0.001 * gain), "L1701", 0, 1e-3 * gain, 1e-3 * gain);
	// 6 
	connection->callAndThrowOnError6716(bu6716_setShuntCal, "bu6716_setShuntCal", CHANNEL_MASK, true);
	bu3100_sleep(50);
	// 7
	errorDetected |= checkValues(CHANNEL_MASK, readValues(bu3416_GAIN_1, CHANNEL_MASK, 0.001 * gain), "L1702", -6e-3 * gain, 1e-3 * gain, 1e-3 * gain);
	// 8
	connection->callAndThrowOnError6716(bu6716_setShuntCal, "bu6716_setShuntCal", CHANNEL_MASK, false);
	connection->callAndThrowOnError6716(bu6716_setPosExcitation, "bu6716_setPosExcitation", CHANNEL_MASK, 0.0);
	connection->callAndThrowOnError6716(bu6716_setNegExcitation, "bu6716_setNegExcitation", CHANNEL_MASK, 0.0);
	setAutoDACPositive(CHANNEL_MASK, 0.0);
	setAutoDACNegative(CHANNEL_MASK, 0.0);
	connection->callAndThrowOnErrorT028(t028_setChannelsConfig, "t028_setChannelsConfig", 0xFFFF, T028_MODE_EXCAL);
	return errorDetected == 0;
}

ShuntTest::ShuntTest(const std::shared_ptr<Communication_6716>& connection) : Abstract6716Test("Shunt", connection) {}
