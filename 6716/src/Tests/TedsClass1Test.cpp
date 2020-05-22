#include "../../include/Tests/TedsClass1Test.h"

Result TedsClass1Test::test() const {
	ViStatus status;
	ViUInt8 segsw_dev;
	ViUInt16 reset_cmd[] = { 0x0300, 0x0000 };

	ViUInt16 read_cmd[] = { 0x0300, 0x02CC, 0x02C3, 0x0200, 0x0100, 0x0100, 0x0300, 0x0000 };
	int i;
	device6716->invokeFunction(bu6716_reloadConfig, "bu6716_reloadConfig", true);
	device6716->invokeFunction(bu6716_setChannelConf, "bu6716_setChannelConf", device6716->channelsStateAsMask(), bu6716_MODE_FULL_BRIDGE, bu6716_GAIN_1, bu6716_COUPLING_DC, bu6716_INP_SRC_FP);
	device6716->configureVoltageReferanceSwitches(0x60);

	device6716->invokeFunction(bu6716_setExcitationMonitor, "bu6716_setExcitationMonitor", bu6716_EXCMON_OFF);

	sender_->waitForConfirmation("Please, make sure all tested channels are connected and click ok.");

	segsw_dev = device6716->readFPGAreg(bu6716_FPGA_SEGSW);
	for (int j = 0; j < 3; j++) {
		log(QString("ITERATION %1/%2").arg(j + 1).arg(3));
		for (auto& channel : device6716->channels()) {
			//1 & 2 or 11
			device6716->isSegment1(channel.index()) ? device6716->writeFPGAreg(bu6716_FPGA_SEGSW, 0x1) : device6716->writeFPGAreg(bu6716_FPGA_SEGSW, 0x0);
			bu3100_sleep(100);
			if (channel.disabled())
				continue;
			channelsResults.at(channel.index()) = Result::VALUE::PASSED;
			log(QString("CHANNEL %1").arg(channel.index()));
			// 3
			auto portExp = device6716->readPortExpander(channel.index());
			device6716->writePortExpander(channel.index(), portExp & 0xafff);  // enable icpgnd and teds1																								// 4
			deviceT028->invokeFunction(t028_setChanConfig, "t028_setChanConfig", channel.index(), T028_MODE_TEDS1);
			bu3100_sleep(100);
			// 5 & 6
			ViUInt16 write_cmd[] = { 0x0300, 0x02CC, 0x0299, 0x0200, 0x0200 | (channel.index() & 0x00FF), 0x0200 | (~channel.index() & 0x00FF), 0x0300, 0x0000 };
			status = device6716->oneWireCommander(write_cmd);

			if (status < VI_SUCCESS) {
				if (status == bu3416_TEDS_NOTREADY)
					log(QString("- ERROR! Cannot write to TEDS, not ready (channel: %1, step 5)").arg(channel.index()));
				if (status == bu3416_TEDS_NOTPRESENT)
					log(QString("- ERROR! Cannot write to TEDS, not present (channel: %1, step 5)").arg(channel.index()));
				channelsResults.at(channel.index()) = Result::VALUE::FAILED;
			}
			bu3100_sleep(20);
			// 7 & 8
			ViUInt16 read_cmd[] = { 0x0300, 0x02CC, 0x02C3, 0x0200, 0x0100, 0x0100, 0x0300, 0x0000 };
			status = device6716->oneWireCommander(read_cmd);
			if (status < VI_SUCCESS) {
				if (status == bu3416_TEDS_NOTREADY)
					log(QString("- ERROR! Cannot read from TEDS, not ready (channel %1, step 7)").arg(channel.index()));
				if (status == bu3416_TEDS_NOTPRESENT)
					log(QString("- ERROR! Cannot read from TEDS, not present (channel %1, step 7)").arg(channel.index()));
				channelsResults.at(channel.index()) = Result::VALUE::FAILED;
			}
			for (i = 0; i < 2; i++) {
				if ((read_cmd[4 + i] & 0x00FF) != (write_cmd[4 + i] & 0x00FF)) {
					log(QString("- ERROR! Scratchpad read value %1 from TEDS is not equal to 0x%2 (channel %3, step 8)").arg(read_cmd[4 + i] & 0x00FF).arg(write_cmd[4 + i] & 0x00FF).arg(channel.index()));
					log(QString("- ERROR! channel %1 failed 0x%2 0x%3").arg(channel.index()).arg(read_cmd[4 + i] & 0x00FF).arg(write_cmd[4 + i] & 0x00FF));
					channelsResults.at(channel.index()) = Result::VALUE::FAILED;
				}
			}
			// 9
			device6716->writePortExpander(channel.index(), portExp);
			bu3100_sleep(50);
			// 10
			status = device6716->oneWireCommander(reset_cmd);

			if (status != bu3416_TEDS_NOTPRESENT) {
				if (status == bu3416_TEDS_READY)
					log(QString("- ERROR! Cannot check TEDS, not ready (channel %1, step 10)").arg(channel.index()));
				else if (status == VI_SUCCESS)
					log(QString("- ERROR! TEDS present but should not be (channel %1, step 10)").arg(channel.index()));
				channelsResults.at(channel.index()) = Result::VALUE::FAILED;
			}
			deviceT028->invokeFunction(t028_setChannelsConfig, "t028_setChannelsConfig", 0xffff, T028_MODE_EXCAL);
			if (channelsResults.at(channel.index()) == Result::VALUE::PASSED)
				log("OK");
		}
	}
	device6716->writeFPGAreg(bu6716_FPGA_SEGSW, segsw_dev);
	log("SCPI/driver functionality test:");
	srand((unsigned int)time(nullptr));
	for (auto& channel : device6716->channels()) {
		unsigned char data[32];
		unsigned char data_wr[32];
		if (channel.disabled())
			continue;
		deviceT028->invokeFunction(t028_setChanConfig, "t028_setChanConfig", channel.index(), T028_MODE_TEDS1);
		log(QString("CHANNEL %1:").arg(channel.index()));
		for (int j = 0; j < 32; j++)
			data_wr[j] = rand();

		deviceT028->invokeFunction(bu6716_writeTEDS, "bu6716_writeTEDS", channel.index(), bu6716_TEDS_CLASS1, 0, data_wr, 32);
		log("  EEPROM (written): ");
		for (int j = 0; j < 32; j++)
			log(QString("%1").arg(data_wr[j]));
		log("");

		deviceT028->invokeFunction(bu6716_readTEDS, "bu6716_readTEDS", channel.index(), bu6716_TEDS_CLASS1, bu6716_TEDS_MEM_EEPROM, data);
		log("  EEPROM (read)   : ");
		for (int j = 0; j < 32; j++)
			log(QString("%1").arg(data[j]));
		log("");
		for (int j = 0; j < 32; j++) {
			if (data[j] != data_wr[j]) {
				log("  ERROR! Data verification failed!");
				channelsResults.at(channel.index()) = Result::VALUE::FAILED;
				break;
			}
		}

		deviceT028->invokeFunction(bu6716_readTEDS, "bu6716_readTEDS", channel.index(), bu6716_TEDS_CLASS1, bu6716_TEDS_MEM_OTPROM, data);
		log("  OTPROM          : ");
		for (int j = 0; j < 8; j++)
			log(QString("%1").arg(data[j]));
		log("");	

		deviceT028->invokeFunction(bu6716_readTEDS, "bu6716_readTEDS", channel.index(), bu6716_TEDS_CLASS1, bu6716_TEDS_MEM_ROM, data);
		log("  ROM             : ");
		for (int j = 0; j < 8; j++)
			log(QString("%1").arg(data[j]));
		log("");

		deviceT028->invokeFunction(t028_setChannelsConfig, "t028_setChannelsConfig", 0xffff, T028_MODE_EXCAL);
	}
	return channelsResult();
}

TedsClass1Test::TedsClass1Test() : AbstractTest6716("Teds Class 1") {}
