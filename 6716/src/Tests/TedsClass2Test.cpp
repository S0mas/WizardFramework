#include "../../include/Tests/TedsClass2Test.h"

void TedsClass2Test::_initTeds2() const {
	device6716->writeI2cNoAddress(0x19, bu6716_TEDS_CMD_DRST);
	device6716->writeI2c(0x19, bu6716_TEDS_CMD_WCFG, bu6716_TEDS_CFG_DFL);
}

bool TedsClass2Test::_resetTeds2(unsigned channel, unsigned step) const {
	unsigned char statusByte;
	ViReal64 startTime = bu3100_getStartTime();
	device6716->writeI2cNoAddress(0x19, bu6716_TEDS_CMD_1WRS);
	bu3100_sleep(50);
	do {
		if (bu3100_getStartTime() - startTime > 1000.0) {
			log(QString("- ERROR! Cannot reset TEDS2, not ready (channel: %1, step %2)").arg(channel).arg(step));
			return false;
		}
		bu3100_sleep(50);
		statusByte = device6716->readI2cNoAddress(0x19);
	} while (statusByte & bu6716_TEDS_STATUS_1WB);

	if (statusByte & bu6716_TEDS_STATUS_SD) {
		log(QString("- ERROR! Cannot reset TEDS2, short (channel: %1, step %2)").arg(channel).arg(step));
		return false;
	}
	if (step < 9) {
		if (!(statusByte & bu6716_TEDS_STATUS_PPD)) {
			log(QString("- ERROR! Cannot reset TEDS2, not present (channel: %1, step %2)").arg(channel).arg(step));
			return false;
		}
	}
	else {
		if (statusByte & bu6716_TEDS_STATUS_PPD) {
			log(QString("- ERROR! TEDS2 present but should not be (channel: %1, step %2)").arg(channel).arg(step));
			return false;
		}
	}
	return true;
}

Result TedsClass2Test::test() const {
	device6716->invokeFunction(bu6716_reloadConfig, "bu6716_reloadConfig", true);
	deviceT028->invokeFunction(t028_setChannelsConfig, "t028_setChannelsConfig", device6716->channelsStateAsMask(), T028_MODE_EXCAL);
	device6716->configureVoltageReferanceSwitches(0x60);
	auto retries = 0;
	auto maxRetries = 3;
	auto segsw_dev = device6716->readFPGAreg(bu6716_FPGA_SEGSW);
	for (auto& channel : device6716->channels()) {
	retrySameChannel:
		if (channel.disabled())
			continue;
		log(QString("CHANNEL %1").arg(channel.index()));
		if (retries == 0)
			sender_->waitForConfirmation(QString("Please connect sensor to channel %1 and click ok").arg(channel.index()));
		//1 or 11
		device6716->isSegment1(channel.index()) ? device6716->writeFPGAreg(bu6716_FPGA_SEGSW, 0x1) : device6716->writeFPGAreg(bu6716_FPGA_SEGSW, 0x0);
		bu3100_sleep(100);

		channelsResults.at(channel.index()) = Result::VALUE::PASSED;
		_initTeds2();

		// 2
		auto portExp = device6716->readPortExpander(channel.index());
		device6716->writePortExpander(channel.index(), (portExp & 0xfffe) | 0x4000);  // enable teds2 and disable shunt cal																							   // 3
		deviceT028->invokeFunction(t028_setChanConfig, "t028_setChanConfig", channel.index(), T028_MODE_TEDS2);
		bu3100_sleep(100);

		// 4
		_resetTeds2(channel.index(), 4);

		// 5
		unsigned char write_data[2] = { channel.index(), ~channel.index() };
		unsigned char read_data[2] = { 0, 0 };
		device6716->writeTeds2(bu6716_1W_CMD_SKIP_ROM);
		device6716->writeTeds2(bu6716_1W_CMD_WRITE_SPD);
		device6716->writeTeds2(0x00);
		for (int i = 0; i < 2; i++)
			device6716->writeTeds2(write_data[i]);

		// 6
		_resetTeds2(channel.index(), 6);

		// 7
		device6716->writeTeds2(bu6716_1W_CMD_SKIP_ROM);
		device6716->writeTeds2(bu6716_1W_CMD_READ_SPD);
		device6716->writeTeds2(0x00);
		for (int i = 0; i < 2; i++)
			read_data[i] = device6716->readTeds2();
		for (int i = 0; i < 2; i++) {
			if (read_data[i] != write_data[i]) {
				log(QString("- ERROR! Read value %1 from TEDS is not equal to %2 (channel: %3, step 7)").arg(read_data[i]).arg(write_data[i]).arg(channel.index()));
				channelsResults.at(channel.index()) = Result::VALUE::FAILED;
			}
		}

		// 8
		_resetTeds2(channel.index(), 8);

		// 9
		device6716->writePortExpander(channel.index(), portExp);
		deviceT028->invokeFunction(t028_setChannelsConfig, "t028_setChannelsConfig", 0xffff, T028_MODE_EXCAL);
		_resetTeds2(channel.index(), 9);
		if (channelsResults.at(channel.index()) == Result::VALUE::PASSED) {
			log(QString("OK, number of retries: %1)").arg(retries));
			retries = 0;
		}
		else if (retries % maxRetries == 0 && retries != 0) {
			if (sender_->waitForAcceptOrDecline(QString("There was an error on channel %1. Retry? Press Cancel to skip to the next channel").arg(channel.index()))) {
				++retries;
				goto retrySameChannel;
			}
			else {
				log(QString("Channel %1 skipped)").arg(channel.index()));
				retries = 0;
			}
		}
		else 
			goto retrySameChannel;
	}
	device6716->writeFPGAreg(bu6716_FPGA_SEGSW, segsw_dev);
	return channelsResult();
	//////////////////////////////////////////////////////////////////
	///////// DRIVER TEST WILL WAIT (problem with write teds function
	log("SCPI/driver functionality test:");
	srand((unsigned int)time(nullptr));
	for (auto& channel : device6716->channels()) {
		unsigned char data[32];
		unsigned char data_wr[32];
		if (channel.disabled())
			continue;
		sender_->waitForConfirmation(QString("Please connect sensor to channel %1 and click ok").arg(channel.index()));

		deviceT028->invokeFunction(t028_setChanConfig, "t028_setChanConfig", channel.index(), T028_MODE_TEDS2);
		log(QString("CHANNEL %1:").arg(channel.index()));
		for (int j = 0; j < 32; j++)
			data_wr[j] = rand();

		deviceT028->invokeFunction(bu6716_writeTEDS, "bu6716_writeTEDS", channel.index(), bu6716_TEDS_CLASS2, 0, data_wr, 32);
		log("  EEPROM (written): ");
		for (int j = 0; j < 32; j++)
			log(QString("%1 ").arg(data_wr[j]));
		log("");

		deviceT028->invokeFunction(bu6716_readTEDS, "bu6716_readTEDS", channel.index(), bu6716_TEDS_CLASS2, bu6716_TEDS_MEM_EEPROM, data);
		log("  EEPROM (read)   : ");
		for (int j = 0; j < 32; j++)
			log(QString("%1 ").arg(data[j]));
		log("");
		for (int j = 0; j < 32; j++) {
			if (data[j] != data_wr[j]) {
				log("  ERROR! Data verification failed!");
				channelsResults.at(channel.index()) = Result::VALUE::FAILED;
				break;
			}
		}

		deviceT028->invokeFunction(bu6716_readTEDS, "bu6716_readTEDS", channel.index(), bu6716_TEDS_CLASS2, bu6716_TEDS_MEM_OTPROM, data);
		log("  OTPROM          : ");
		for (int j = 0; j < 8; j++)
			log(QString("%1").arg(data[j]));
		log("");

		deviceT028->invokeFunction(bu6716_readTEDS, "bu6716_readTEDS", channel.index(), bu6716_TEDS_CLASS2, bu6716_TEDS_MEM_ROM, data);
		log("  ROM             : ");
		for (int j = 0; j < 8; j++)
			log(QString("%1").arg(data[j]));
		log("");

		deviceT028->invokeFunction(t028_setChannelsConfig, "t028_setChannelsConfig", (unsigned short)0xffff, T028_MODE_EXCAL);
	}
	return channelsResult();
}

TedsClass2Test::TedsClass2Test() : AbstractTest6716("Teds Class 2") {}
