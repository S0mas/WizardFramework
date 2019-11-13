#include "../include/Tests/SCUErrorTest.h"
#include "../include/defines.h"

bool SCUErrorTest::test() const {
	
	connection->callAndThrowOnError6716(bu6716_reloadConfig, "bu6716_reloadConfig", true);
	connection->callAndThrowOnError6716(bu6716_setChannelConf, "bu6716_setChannelConf", CHANNEL_MASK, bu6716_MODE_DI, bu6716_GAIN_1, bu6716_COUPLING_DC, bu6716_INP_SRC_FP);
	//volt ref setup
	auto reg = connection->readFPGAreg(bu6716_FPGA_SEGCONF);
	log(QString("reg value is %1").arg(QString::number(reg, 2)));
	connection->writeFPGAreg(bu6716_FPGA_SEGCONF, 0x60);
	reg = connection->readFPGAreg(bu6716_FPGA_SEGCONF);
	log(QString("reg value is %1").arg(QString::number(reg, 2)));
	bu3100_sleep(50);

	connection->callAndThrowOnError6716(bu6716_setExcitationMonitor, "bu6716_setExcitationMonitor", bu6716_EXCMON_OFF);
	connection->callAndThrowOnErrorT028(t028_setChannelsConfig, "t028_setChannelsConfig", CHANNEL_MASK, T028_MODE_EXCAL);
	for (int i = 0; i < bu6716_NUM_CHAN; i++) {
		ViInt16 sts, stsAll[bu6716_NUM_CHAN];
		if (!(CHANNEL_MASK & (1 << i)))
			continue;
		ViUInt16 channelMask = 1 << i;
		log(QString("Channel %1").arg(i + 1));
		log("- IEPE");
		for (int s = 0; s < 2; s++) {
			// Enable IEPE on T028 and on 6716 - the IEPE current shall be detected
			connection->callAndThrowOnErrorT028(t028_setChannelsConfig, "t028_setChannelsConfig", channelMask, T028_MODE_ICP);
			connection->callAndThrowOnError6716(bu6716_setMode, "bu6716_setMode", channelMask, bu6716_MODE_IEPE);
			connection->callAndThrowOnError6716(bu6716_getCurrentStatus, "bu6716_getCurrentStatus", channelMask, &sts, nullptr);
			if (sts != bu6716_IEPE_OK) {
				log("  IEPE current NOT detected, but should be");
				channelsErrorsMask |= (1 << i);
			}
			// Disable IEPE on T028, so the error should happen. Enable IEPE to SCU_ERR in iteration s==1.
			connection->callAndThrowOnError6716(bu6716_setIEPEScuErrorMask, "bu6716_setIEPEScuErrorMask", 0xffff, s == 1);
			connection->callAndThrowOnErrorT028(t028_setChannelsConfig, "t028_setChannelsConfig", CHANNEL_MASK, T028_MODE_EXCAL);
			connection->callAndThrowOnError6716(bu6716_getCurrentStatus, "bu6716_getCurrentStatus", channelMask, &sts, nullptr);
			if (sts == bu6716_IEPE_OK) {
				log("  IEPE current detected, but shouldn't be");
				channelsErrorsMask |= (1 << i);
			}
			auto dsr = readDSR();
			if (s == 0) {
				if (dsr & 0x08) {
					log("  SCU_ERR detected, but shouldn't be in iteration s==0");
					channelsErrorsMask |= (1 << i);
				}
			}
			else {
				if (!(dsr & 0x08)) {
					log("  SCU_ERR not detected, but should be in iteration s==1");
					channelsErrorsMask |= (1 << i);
				}
			}
			connection->callAndThrowOnError6716(bu6716_getLatchedStatus, "bu6716_getLatchedStatus", stsAll, nullptr);
			for (int j = 0; j < bu6716_NUM_CHAN; j++) {
				if (i == j) {
					if (stsAll[j] != 1) {
						log("  No IEPE error detected, but should be");
						channelsErrorsMask |= (1 << i);
					}
				}
				else {
					if (stsAll[j] != 0) {
						log("  IEPE error detected, but shouldn't be");
						channelsErrorsMask |= (1 << i);
					}
				}
			}
			connection->callAndThrowOnError6716(bu6716_getLatchedStatus, "bu6716_getLatchedStatus", stsAll, nullptr);
			for (int j = 0; j < bu6716_NUM_CHAN; j++) {
				if (stsAll[j] != 0) {
					log("  IEPE error detected, but shouldn't be");
					channelsErrorsMask |= (1 << i);
				}
			}
			connection->callAndThrowOnError6716(bu6716_setMode, "bu6716_setMode", channelMask, bu6716_MODE_DI);
			connection->callAndThrowOnError6716(bu6716_setIEPEScuErrorMask, "bu6716_setIEPEScuErrorMask", 0xffff, false);
		}
		if (!(channelsErrorsMask & (1 << i)))
			log("  OK");
	}
	return channelsErrorsMask == 0;
}

SCUErrorTest::SCUErrorTest(const std::shared_ptr<Communication_6716>& connection) : Abstract6716Test("SCU Error", connection) {}