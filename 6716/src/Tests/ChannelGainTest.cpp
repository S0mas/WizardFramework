#include "../include/Tests/ChannelGainTest.h"
#include "../include/defines.h"

bool ChannelGainTest::test() const {
	ViReal64 calculatedGain;
	
	ViReal64 vrefSet;
	const ViInt16  gains[4] = { bu6716_GAIN_1, bu6716_GAIN_10, bu6716_GAIN_100, bu6716_GAIN_1000 };
	const ViReal64 volts[4] = { 9.0, 0.9, 0.09, 0.009 };
	const ViReal64 limits[4] = { 10e-3, 10e-3, 50e-3, 200e-3 };
	const ViReal64 gainLimits[4] = { 0.05, 0.05, 0.5, 15 };//changed by S. Power and K. Sommerfeld
	ViReal64 vref;
	char limitName[32];
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
	configureVoltageReferanceSwitches(0x64);
	for (int i = 0; i < bu3416_NUM_CHAN; i++) {
		if (!(CHANNEL_MASK & (1 << i)))
			continue;
		ViUInt16 channelMask = 1 << i;  // Only one channel at a time
		log(QString("* CHANNEL = %1").arg(i + 1));
		connection->callAndThrowOnError6716(bu6716_setInputSrc, "bu6716_setInputSrc", i + 1, bu6716_INP_SRC_VREF);
		for (int j = 0; j < 4; j++) {
			log(QString("* GAIN = %1").arg(gains[j]));
			connection->callAndThrowOnError6716(bu6716_setGain, "bu6716_setGain", channelMask, gains[j]);
			for (int k = 0; k < 2; k++) {
				vref = volts[j] * (1.0 - 2 * k);
				connection->callAndThrowOnError6100(bu6100_setVoltRefOutput, "bu6100_setVoltRefOutput", vref);
				connection->callAndThrowOnError6100(bu6100_getVoltRefOutput, "bu6100_getVoltRefOutput", &vrefSet);

				//log("  VREF = %.6lf V  (set=%.6lf V)", vref, vrefSet);
				bu3100_sleep(50);
				auto value = readValue_oneChannel(bu3416_GAIN_1, i + 1, 0.1);
				sprintf(limitName, "L19%02d", j + 1);
				channelsErrorsMask |= checkValue_oneChannel(i + 1, value, limitName, vrefSet * gains[j], limits[j], limits[j], true);
				calculatedGain = value / vrefSet;
				sprintf(limitName, "L19%02d", j + 5);
				channelsErrorsMask |= checkValue_oneChannel(i + 1, calculatedGain, limitName, gains[j], gainLimits[j], gainLimits[j], true, true);
				connection->callAndThrowOnError6100(bu6100_setVoltRefOutput, "bu6100_setVoltRefOutput", 0.0);
			}
		}
		connection->callAndThrowOnError6716(bu6716_setInputSrc, "bu6716_setInputSrc", i + 1, bu6716_INP_SRC_FP);
	}
	// Restore
	connection->callAndThrowOnError6100(bu6100_setVoltRefOutput, "bu6100_setVoltRefOutput", 0.0);
	for (int i = 0, n = 0; i < bu6716_NUM_CHAN; i++) {
		if (CHANNEL_MASK & (1 << i)) {
			bu3100_sleep(20);
			setAutoDACNegative(CHANNEL_MASK, autoDacNeg[n]);
			setAutoDACPositive(CHANNEL_MASK, autoDacPos[n]);
			n++;
		}
	}
	return channelsErrorsMask == 0;
}

ChannelGainTest::ChannelGainTest(const std::shared_ptr<Communication_6716>& connection) : Abstract6716Test("Channel Gain", connection) {}