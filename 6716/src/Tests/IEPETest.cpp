#include "../include/Tests/IEPETest.h"
#include "../include/defines.h"

bool IEPETest::test() const {
	int errorDetected = 0;
	ViInt16 iepeStatus[bu6716_NUM_CHAN];
	ViUInt16 iepeStatus_mask;
	connection->callAndThrowOnError6716(bu6716_reloadConfig, "bu6716_reloadConfig", true);
	// 1
	connection->callAndThrowOnError6716(bu6716_setChannelConf, "bu6716_setChannelConf", CHANNEL_MASK, bu6716_MODE_DI, bu6716_GAIN_1, bu6716_COUPLING_DC, bu6716_INP_SRC_FP);
	connection->callAndThrowOnError6716(bu6716_setExcitationMonitor, "bu6716_setExcitationMonitor", bu6716_EXCMON_OFF);
	connection->callAndThrowOnErrorT028(t028_setChannelsConfig, "t028_setChannelsConfig", CHANNEL_MASK, T028_MODE_EXCAL);
	for (int i = 0; i < bu6716_NUM_CHAN; i++) {
		if (!(CHANNEL_MASK & (1 << i)))
			continue;
		ViUInt16 channelMask = 1 << i;
		// 2
		printf("Channel %d\n", i + 1);
		connection->callAndThrowOnErrorT028(t028_setRelay, "t028_setRelay", i + 1, T028_RELAY_4, T028_ON);
		connection->callAndThrowOnErrorT028(t028_setRelay, "t028_setRelay", i + 1, T028_RELAY_6, T028_ON);
		connection->callAndThrowOnError6716(bu6716_setMode, "bu6716_setMode", channelMask, bu6716_MODE_IEPE);

		// Force DC coupling
		connection->writePortExpander(i + 1, connection->readPortExpander(i + 1) & 0xfcff);
		bu3100_sleep(50);
		// 3
		auto value = readValue_oneChannel(bu3416_GAIN_10, i + 1, 0.1);
		errorDetected |= checkValue_oneChannel(i + 1, value, "L2101", 0.535, 0.135, 0.135);
		connection->callAndThrowOnError6716(bu6716_setMode, "bu6716_setMode", channelMask, bu6716_MODE_IEPE);  // Sets AC-pos, DC-neg coupling 
		bu3100_sleep(10);
		// Check mode, just in case...
		ViInt16 channelMode;
		connection->callAndThrowOnError6716(bu6716_getMode, "bu6716_getMode", channelMask, &channelMode);
		if (channelMode != bu6716_MODE_IEPE)
			printf("ERROR - server/driver - mode is not bu6716_MODE_IEPE !!!\n");
		// 4
		printf("Channel %d (IEPE connected)\n", i + 1);
		connection->callAndThrowOnError6716(bu6716_getCurrentStatus, "bu6716_getCurrentStatus", 0xffff, iepeStatus, nullptr);
		iepeStatus_mask = 0;
		for (int j = 0; j < bu6716_NUM_CHAN; j++)
			if (iepeStatus[j] == bu6716_IEPE_OK)
				iepeStatus_mask |= (1 << j);
		if (iepeStatus[i] == bu6716_IEPE_OK) {
			printf("- Detected valid IEPE current (OK)\n");
		}
		else if (iepeStatus[i] == bu6716_IEPE_NO_CURRENT) {
			printf("- ERROR. Not valid IEPE current detected (channel: %d, icp_st: 0x%04hX step 4)\n", i + 1, iepeStatus_mask);
			errorDetected |= channelMask;
		}
		else {
			if (iepeStatus_mask & (~(1 << i))) {
				printf("- ERROR. Valid IEPE current detected on others channels (channel: %d, icp_st: 0x%04hX step 4)\n", i + 1, iepeStatus_mask);
				errorDetected |= channelMask;
			}
		}
		// 6
		connection->callAndThrowOnErrorT028(t028_setRelay, "t028_setRelay", i + 1, T028_RELAY_6, T028_OFF);
		// 7
		printf("Channel %d (IEPE disconnected)\n", i + 1);
		connection->callAndThrowOnError6716(bu6716_getCurrentStatus, "bu6716_getCurrentStatus", 0xffff, iepeStatus, nullptr);
		iepeStatus_mask = 0;
		for (int j = 0; j < bu6716_NUM_CHAN; j++)
			if (iepeStatus[j] == bu6716_IEPE_OK)
				iepeStatus_mask |= (1 << j);
		if (iepeStatus[i] == bu6716_IEPE_NO_CURRENT) {
			printf("- IEPE current not detected (OK)\n");
		}
		else if (iepeStatus[i] == bu6716_IEPE_OK) {
			printf("- ERROR. Valid IEPE current detected (channel: %d, icp_st: 0x%04hX step 7)\n", i + 1, iepeStatus_mask);
			errorDetected |= channelMask;
		}
		else {
			if (iepeStatus_mask & (~(1 << i))) {
				printf("- ERROR. Valid IEPE current detected on others channels (channel: %d, icp_st: 0x%04hX step 7)\n", i + 1, iepeStatus_mask);
				errorDetected |= channelMask;
			}
		}
		connection->callAndThrowOnError6716(bu6716_setChannelConf, "bu6716_setChannelConf", channelMask, bu6716_MODE_DI, bu6716_GAIN_1, bu6716_COUPLING_DC, bu6716_INP_SRC_FP);
		connection->callAndThrowOnErrorT028(t028_setRelay, "t028_setRelay", i + 1, T028_RELAY_4, T028_OFF);
		printf("\n");
	}
	return errorDetected == 0;
}

IEPETest::IEPETest(const std::shared_ptr<Communication_6716>& connection) : Abstract6716Test("IEPE", connection) {}