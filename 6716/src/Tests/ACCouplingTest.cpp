#include "../include/Tests/ACCouplingTest.h"
#include "../include/defines.h"

bool ACCouplingTest::test() const {
	int errorDetected = 0;
	const ViReal64 vref[2] = { 9.0, -9.0 };
	ViReal64 vrefSet;
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
	bu3100_sleep(100);
	// 3
	connection->callAndThrowOnError6716(bu6716_setVoltRefMode, "bu6716_setVoltRefMode", bu6716_VREF_MODE_INT_MONITOR_ON);
	for (int i = 0; i < bu3416_NUM_CHAN; i++) {
		if (!(CHANNEL_MASK & (1 << i)))
			continue;
		ViUInt16 channelMask = 1 << i;  // Only one channel at a time
		printf("CHANNEL %d\n", i + 1);
		connection->callAndThrowOnError6716(bu6716_setInputSrc, "bu6716_setInputSrc", i + 1, bu6716_INP_SRC_VREF);
#ifdef AUTO_DAC  // Seems it's not needed. Not described in the TP doc, Stephen also doesn't remember why it was added in 5716 prodtest code...
		ViReal64 autoDAC[2] = { 0.0, 0.0 };

		for (int j = 0; j < 2; j++) {
			// 3 & 4
			BU6716_CHECK_ERR(bu6716_setVoltRefOutput, connection->getVi6716(), vref[j]);
			BU6716_CHECK_ERR(bu6716_getVoltRefOutput, connection->getVi6716(), &vrefSet);
			bu3100_sleep(50);
			// 6 & 7
			{
				ViUInt16 data16;
				BU6716_CHECK_ERR(readPortExpander, connection->getVi6716(), i + 1, &data16);
				if (j == 0) {
					BU6716_CHECK_ERR(writePortExpander(connection->getVi6716(), i + 1, , data16 & 0xfcff) | 0x0200);  // ACpos
				}
				else {
					BU6716_CHECK_ERR(writePortExpander(connection->getVi6716(), i + 1, , data16 & 0xfcff) | 0x0100);  // ACneg
				}
				// Debug
				//BU6716_CHECK_ERR(readPortExpander, connection->getVi6716(), i + 1, &data16);
				//printf("port expander: 0x%04hx\n", data16);
			}
			bu3100_sleep(5000);  // Allow full discharge
			BU3416_CHECK_ERR(readValue_oneChannel, env, bu3416_GAIN_1, i + 1, 0.1, &value, 5000, 100);
			autoDAC[j] = -value / 0.00305 * bu6716_DAC_AUTO_GAIN;
			BU6716_CHECK_ERR(bu6716_setCoupling, connection->getVi6716(), channelMask, bu6716_COUPLING_DC);
		}
		printf("autoDacpos = %lf, autoDACneg = %lf\n", autoDAC[0], autoDAC[1]);
#endif
		for (int j = 0; j < 2; j++) {
			// 3 & 4
			connection->callAndThrowOnError6716(bu6716_setVoltRefOutput, "bu6716_setVoltRefOutput", vref[j]);
			connection->callAndThrowOnError6716(bu6716_getVoltRefOutput, "bu6716_getVoltRefOutput", &vrefSet);
			bu3100_sleep(50);
			// 5
			auto value = readValue_oneChannel(bu3416_GAIN_1, i + 1, 0.1);
			errorDetected |= checkValue_oneChannel(i + 1, value, "L2003", vrefSet, 0.01, 0.01, true);

#ifdef AUTO_DAC
			setAutoDAC(channelMask, AUTO_DAC_POSITIVE, autoDAC[j]);  // In 5716 test code just the positive was set...
			bu3100_sleep(5000);
#endif
			// 6 & 7
			{
				ViUInt16 data16 = connection->readPortExpander(i + 1);
				ViUInt16 acPolarization = (j == 0) ? 0x0200 : 0x0100;
				connection->writePortExpander(i + 1, (data16 & 0xfcff) | acPolarization);  // ACpos
			}
			bu3100_sleep(1500);  // Discharge time - must be 1.5 sec
			// 8
			value = readValue_oneChannel(bu3416_GAIN_1, i + 1, 0.1, 5000.0, 100);
			if (j == 0)
				errorDetected |= checkValue_oneChannel(i + 1, value, "L2001", 3.0, 1, 1, true);
			else
				errorDetected |= checkValue_oneChannel(i + 1, value, "L2002", -3.0, 1, 1, true);
			connection->callAndThrowOnError6716(bu6716_setCoupling, "bu6716_setCoupling", channelMask, bu6716_COUPLING_DC);
			bu3100_sleep(50);
		}
		connection->callAndThrowOnError6716(bu6716_setInputSrc, "bu6716_setInputSrc", i + 1, bu6716_INP_SRC_FP);
	}
	// 16, Restore
	connection->callAndThrowOnError6716(bu6716_setVoltRefOutput, "bu6716_setVoltRefOutput", 0.0);
	connection->callAndThrowOnError6716(bu6716_setVoltRefMode, "bu6716_setVoltRefMode", bu6716_VREF_MODE_EXT);
	for (int i = 0, n = 0; i < bu6716_NUM_CHAN; i++) {
		if (CHANNEL_MASK & (1 << i)) {
			setAutoDACNegative(CHANNEL_MASK, autoDacNeg[n]);
			setAutoDACPositive(CHANNEL_MASK, autoDacPos[n]);
			n++;
		}
	}
	return errorDetected == 0;
}

ACCouplingTest::ACCouplingTest(const std::shared_ptr<Communication_6716>& connection) : Abstract6716Test("AC Coupling", connection) {}