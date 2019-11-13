#include "../include/Tests/TedsClass2Test.h"
#include "../include/defines.h"

bool TedsClass2Test::_initTeds2() const {
	connection->writeI2C_no_addr(0x19, bu6716_TEDS_CMD_DRST);
	connection->writeI2C(0x19, bu6716_TEDS_CMD_WCFG, bu6716_TEDS_CFG_DFL);
	return true;
}

bool TedsClass2Test::_resetTeds2(unsigned channel, unsigned step) const {
	unsigned char statusByte;
	ViReal64 startTime = bu3100_getStartTime();
	connection->writeI2C_no_addr(0x19, bu6716_TEDS_CMD_1WRS);
	do {
		if (bu3100_getStartTime() - startTime > 1000.0) {
			log(QString("- ERROR! Cannot reset TEDS2, not ready (channel: %1, step %2)\n").arg(channel).arg(step));
			return false;
		}
		statusByte = connection->readI2C_no_addr(0x19);
	} while (statusByte & bu6716_TEDS_STATUS_1WB);

	if (statusByte & bu6716_TEDS_STATUS_SD) {
		log(QString("- ERROR! Cannot reset TEDS2, short (channel: %1, step %2)\n").arg(channel).arg(step));
		return false;
	}
	if (step < 9) {
		if (!(statusByte & bu6716_TEDS_STATUS_PPD)) {
			log(QString("- ERROR! Cannot reset TEDS2, not present (channel: %1, step %2)\n").arg(channel).arg(step));
			return false;
		}
	}
	else {
		if (statusByte & bu6716_TEDS_STATUS_PPD) {
			log(QString("- ERROR! TEDS2 present but should not be (channel: %1, step %2)\n").arg(channel).arg(step));
			return false;
		}
	}
	return true;
}

bool TedsClass2Test::test() const {
	int errorDetected = 0;
	unsigned char write_data[2];
	unsigned char read_data[2];
	connection->callAndThrowOnError6716(bu6716_reloadConfig, "bu6716_reloadConfig", true);
	connection->callAndThrowOnErrorT028(t028_setChannelsConfig, "t028_setChannelsConfig", CHANNEL_MASK, T028_MODE_EXCAL);
	configureVoltageReferanceSwitches(0x60);

	auto segsw_dev = connection->readFPGAreg(bu6716_FPGA_SEGSW);
	for (int s = 0; s < 2; s++) {
		if (s == 0)
			// 1
			connection->writeFPGAreg(bu6716_FPGA_SEGSW, 0x1);
		else
			// 11
			connection->writeFPGAreg(bu6716_FPGA_SEGSW, 0x0);
		bu3100_sleep(100);
		if (!_initTeds2())
			errorDetected |= 0xffff;
		for (int c = 0; c < 8; c++) {
			unsigned short portExp;
			unsigned char ch = c + 1 + s * 8;
			if (!(CHANNEL_MASK & 1 << (ch - 1)))
				continue;

			waitForUserAction(QString("Please connect sensor to channel %1 and click ok").arg(ch), UserActionType::CONFIRMATION);

			log(QString("CHANNEL %1\n").arg(ch));
			// 2
			portExp = connection->readPortExpander(ch);
			connection->writePortExpander(ch, (portExp & 0xfffe) | 0x4000);  // enable teds2 and disable shunt cal																							   // 3
			connection->callAndThrowOnErrorT028(t028_setChanConfig, "t028_setChanConfig", ch, T028_MODE_TEDS2);
			bu3100_sleep(100);
			// 4
			_resetTeds2(c+1, 4);
			// 5
			write_data[0] = (ViUInt8)ch;
			write_data[1] = (ViUInt8)(~ch);
			read_data[0] = 0;
			read_data[1] = 0;
			connection->writeTeds2(bu6716_1W_CMD_SKIP_ROM);
			connection->writeTeds2(bu6716_1W_CMD_WRITE_SPD);
			connection->writeTeds2(0x00);
			for (int i = 0; i < 2; i++)
				connection->writeTeds2(write_data[i]);
			// 6
			_resetTeds2(c+1, 6);
			// 7
			connection->writeTeds2(bu6716_1W_CMD_SKIP_ROM);
			connection->writeTeds2(bu6716_1W_CMD_READ_SPD);
			connection->writeTeds2(0x00);
			for (int i = 0; i < 2; i++)
				read_data[i] = connection->readTeds2();
			for (int i = 0; i < 2; i++) {
				if (read_data[i] != write_data[i]) {
					log(QString("- ERROR! Read value %1 from TEDS is not equal to %2 (channel: %3, step 7)\n").arg(read_data[i]).arg(write_data[i]).arg(c+1));
					errorDetected |= (1 << (ch - 1));
				}
			}
			// 8
			_resetTeds2(c+1, 8);
			// 9
			connection->writePortExpander(ch, portExp);
			connection->callAndThrowOnErrorT028(t028_setChannelsConfig, "t028_setChannelsConfig", 0xffff, T028_MODE_EXCAL);
			_resetTeds2(c+1, 9);
			if (!(errorDetected & (1 << (ch - 1))))
				log("  OK\n");
		}
	}
	connection->writeFPGAreg(bu6716_FPGA_SEGSW, segsw_dev);
	bu3100_sleep(250);
	return errorDetected == 0;

	///////// DRIVER TEST WILL WAIT (problem with write teds function
	log("\nSCPI/driver functionality test:\n");
	srand((unsigned int)time(nullptr));
	for (int i = 0; i < bu6716_NUM_CHAN; i++) {
		unsigned char data[32];
		unsigned char data_wr[32];
		if (!(CHANNEL_MASK & (1 << i)))
			continue;
		waitForUserAction(QString("Please connect sensor to channel %1 and click ok").arg(i+1), UserActionType::CONFIRMATION);

		connection->callAndThrowOnErrorT028(t028_setChanConfig, "t028_setChanConfig", i + 1, T028_MODE_TEDS2);
		log(QString("CHANNEL %1:\n").arg(i + 1));
		for (int j = 0; j < 32; j++)
			data_wr[j] = rand();
		if (bu6716_writeTEDS(connection->getVi6716(), i + 1, bu6716_TEDS_CLASS2, 0, data_wr, 32) == VI_SUCCESS) {
			log("  EEPROM (written): ");
			for (int j = 0; j < 32; j++)
				log(QString("%1 ").arg(data_wr[j]));
			log("\n");
		}
		else {
			ViInt32 code;
			ViChar message[1024] = { 0 };
			bu6716_error_query(connection->getVi6716(), &code, message);
			log(QString("  EEPROM: ERROR! %1\n").arg(message));
			errorDetected |= (1 << i);
		}
		if (bu6716_readTEDS(connection->getVi6716(), i + 1, bu6716_TEDS_CLASS2, bu6716_TEDS_MEM_EEPROM, data) == VI_SUCCESS) {
			log("  EEPROM (read)   : ");
			for (int j = 0; j < 32; j++)
				log(QString("%1 ").arg(data[j]));
			log("\n");
			for (int j = 0; j < 32; j++) {
				if (data[j] != data_wr[j]) {
					log("  ERROR! Data verification failed!\n");
					errorDetected |= (1 << i);
					break;
				}
			}
		}
		else {
			ViInt32 code;
			ViChar message[1024] = { 0 };
			bu6716_error_query(connection->getVi6716(), &code, message);
			log(QString("  EEPROM: ERROR! %1\n").arg(message));
			errorDetected |= (1 << i);
		}
		if (bu6716_readTEDS(connection->getVi6716(), i + 1, bu6716_TEDS_CLASS2, bu6716_TEDS_MEM_OTPROM, data) == VI_SUCCESS) {
			log("  OTPROM          : ");
			for (int j = 0; j < 8; j++)
				log(QString("%1 ").arg(data[j]));
			log("\n");
		}
		else {
			ViInt32 code;
			ViChar message[1024] = { 0 };
			bu6716_error_query(connection->getVi6716(), &code, message);
			log(QString("  OTPROM: ERROR! %1\n").arg(message));
			errorDetected |= (1 << i);
		}
		if (bu6716_readTEDS(connection->getVi6716(), i + 1, bu6716_TEDS_CLASS2, bu6716_TEDS_MEM_ROM, data) == VI_SUCCESS) {
			log("  ROM             : ");
			for (int j = 0; j < 8; j++)
				log(QString("%1 ").arg(data[j]));
			log("\n");
		}
		else {
			ViInt32 code;
			ViChar message[1024] = { 0 };
			bu6716_error_query(connection->getVi6716(), &code, message);
			log(QString("  ROM: ERROR! %1\n").arg(message));
			errorDetected |= (1 << i);
		}
		connection->callAndThrowOnErrorT028(t028_setChannelsConfig, "t028_setChannelsConfig", (unsigned short)0xffff, T028_MODE_EXCAL);
	}
	bu3100_sleep(250);
	return errorDetected == 0;
}
#undef CALL
TedsClass2Test::TedsClass2Test(const std::shared_ptr<Communication_6716>& connection) : Abstract6716Test("Teds Class 2", connection) {}