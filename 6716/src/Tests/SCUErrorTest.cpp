#include "../include/Tests/SCUErrorTest.h"
#include "../include/defines.h"

bool SCUErrorTest::test() const {
	int errorDetected = 0;
	connection->callAndThrowOnError6716(bu6716_reloadConfig, "bu6716_reloadConfig", true);
	connection->callAndThrowOnError6716(bu6716_setChannelConf, "bu6716_setChannelConf", CHANNEL_MASK, bu6716_MODE_DI, bu6716_GAIN_1, bu6716_COUPLING_DC, bu6716_INP_SRC_FP);
	connection->callAndThrowOnError6716(bu6716_setExcitationMonitor, "bu6716_setExcitationMonitor", bu6716_EXCMON_OFF);
	connection->callAndThrowOnErrorT028(t028_setChannelsConfig, "t028_setChannelsConfig", CHANNEL_MASK, T028_MODE_EXCAL);
	for (int i = 0; i < bu6716_NUM_CHAN; i++) {
		ViInt16 sts, stsAll[bu6716_NUM_CHAN];
		if (!(CHANNEL_MASK & (1 << i)))
			continue;
		ViUInt16 channelMask = 1 << i;
		printf("Channel %d\n", i + 1);
		printf("- IEPE\n");
		for (int s = 0; s < 2; s++) {
			// Enable IEPE on T028 and on 6716 - the IEPE current shall be detected
			connection->callAndThrowOnErrorT028(t028_setChannelsConfig, "t028_setChannelsConfig", channelMask, T028_MODE_ICP);
			connection->callAndThrowOnError6716(bu6716_setMode, "bu6716_setMode", channelMask, bu6716_MODE_IEPE);
			connection->callAndThrowOnError6716(bu6716_getCurrentStatus, "bu6716_getCurrentStatus", channelMask, &sts, nullptr);
			if (sts != bu6716_IEPE_OK) {
				printf("  IEPE current NOT detected, but should be\n");
				errorDetected |= (1 << i);
			}
			// Disable IEPE on T028, so the error should happen. Enable IEPE to SCU_ERR in iteration s==1.
			connection->callAndThrowOnError6716(bu6716_setIEPEScuErrorMask, "bu6716_setIEPEScuErrorMask", 0xffff, s == 1);
			connection->callAndThrowOnErrorT028(t028_setChannelsConfig, "t028_setChannelsConfig", CHANNEL_MASK, T028_MODE_EXCAL);
			connection->callAndThrowOnError6716(bu6716_getCurrentStatus, "bu6716_getCurrentStatus", channelMask, &sts, nullptr);
			if (sts == bu6716_IEPE_OK) {
				printf("  IEPE current detected, but shouldn't be\n");
				errorDetected |= (1 << i);
			}
			auto dsr = readDSR();
			if (s == 0) {
				if (dsr & 0x08) {
					printf("  SCU_ERR detected, but shouldn't be in iteration s==0\n");
					errorDetected |= (1 << i);
				}
			}
			else {
				if (!(dsr & 0x08)) {
					printf("  SCU_ERR not detected, but should be in iteration s==1\n");
					errorDetected |= (1 << i);
				}
			}
			connection->callAndThrowOnError6716(bu6716_getLatchedStatus, "bu6716_getLatchedStatus", stsAll, nullptr);
			for (int j = 0; j < bu6716_NUM_CHAN; j++) {
				if (i == j) {
					if (stsAll[j] != 1) {
						printf("  No IEPE error detected, but should be\n");
						errorDetected |= (1 << i);
					}
				}
				else {
					if (stsAll[j] != 0) {
						printf("  IEPE error detected, but shouldn't be\n");
						errorDetected |= (1 << i);
					}
				}
			}
			connection->callAndThrowOnError6716(bu6716_getLatchedStatus, "bu6716_getLatchedStatus", stsAll, nullptr);
			for (int j = 0; j < bu6716_NUM_CHAN; j++) {
				if (stsAll[j] != 0) {
					printf("  IEPE error detected, but shouldn't be\n");
					errorDetected |= (1 << i);
				}
			}
			connection->callAndThrowOnError6716(bu6716_setMode, "bu6716_setMode", channelMask, bu6716_MODE_DI);
			connection->callAndThrowOnError6716(bu6716_setIEPEScuErrorMask, "bu6716_setIEPEScuErrorMask", 0xffff, false);
		}
		if (!(errorDetected & (1 << i)))
			printf("  OK\n");
	}
	return errorDetected == 0;
}

SCUErrorTest::SCUErrorTest(const std::shared_ptr<Communication_6716>& connection) : Abstract6716Test("SCU Error", connection) {}