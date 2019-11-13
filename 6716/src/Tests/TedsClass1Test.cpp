#include "../include/Tests/TedsClass1Test.h"
#include "../include/defines.h"

bool TedsClass1Test::test() const {
	ViStatus status;
	

	ViUInt8 segsw_dev;
	ViUInt16 reset_cmd[] = { 0x0300, 0x0000 };
	ViUInt16 write_cmd[] = { 0x0300, 0x02CC, 0x0299, 0x0200, 0x0200, 0x0200, 0x0300, 0x0000 };
	ViUInt16 read_cmd[] = { 0x0300, 0x02CC, 0x02C3, 0x0200, 0x0100, 0x0100, 0x0300, 0x0000 };
	int i;
	connection->callAndThrowOnError6716(bu6716_reloadConfig, "bu6716_reloadConfig", true);
	connection->callAndThrowOnError6716(bu6716_setChannelConf, "bu6716_setChannelConf", CHANNEL_MASK, bu6716_MODE_FULL_BRIDGE, bu6716_GAIN_1, bu6716_COUPLING_DC, bu6716_INP_SRC_FP);
	configureVoltageReferanceSwitches(0x60);

	connection->callAndThrowOnError6716(bu6716_setExcitationMonitor, "bu6716_setExcitationMonitor", bu6716_EXCMON_OFF);
	segsw_dev = connection->readFPGAreg(bu6716_FPGA_SEGSW);
	for (int j = 0; j < 3; j++) {
		log(QString("ITERATION %1/%2").arg(j + 1).arg(3));
		for (int s = 0; s < 2; s++) {
			if (s == 0) {
				// 1 & 2
				connection->writeFPGAreg(bu6716_FPGA_SEGSW, 0x1); 
			}
			else {
				// 12
				connection->writeFPGAreg(bu6716_FPGA_SEGSW, 0x0);
			}
			bu3100_sleep(100);
			for (int c = 0; c < 8; c++) {
				unsigned short portExp = 0;
				unsigned char ch = c + 1 + s * 8;

				if (!(CHANNEL_MASK & 1 << (ch - 1)))
					continue;
				log(QString("CHANNEL %1").arg(ch));
				// 3
				portExp = connection->readPortExpander(ch);
				connection->writePortExpander(ch, portExp & 0xafff);  // enable icpgnd and teds1																								// 4
				connection->callAndThrowOnErrorT028(t028_setChanConfig, "t028_setChanConfig", ch, T028_MODE_TEDS1);
				bu3100_sleep(100);
				// 5 & 6
				write_cmd[4] = 0x0200 | (ch & 0x00FF);  // reset on previous commands...
				write_cmd[5] = 0x0200 | (~ch & 0x00FF);
				status = connection->_1wire_commander(write_cmd);

				if (status < VI_SUCCESS) {
					if (status == bu3416_TEDS_NOTREADY)
						log(QString("- ERROR! Cannot write to TEDS, not ready (channel: %1, step 5)").arg(ch));

					if (status == bu3416_TEDS_NOTPRESENT)
						log(QString("- ERROR! Cannot write to TEDS, not present (channel: %1, step 5)").arg(ch));

					channelsErrorsMask |= (1 << (ch - 1));
				}
				bu3100_sleep(20);
				// 7 & 8
				read_cmd[4] = 0x0100;  // reset on previous commands...
				read_cmd[5] = 0x0100;
				status = connection->_1wire_commander(read_cmd);
				if (status < VI_SUCCESS) {
					if (status == bu3416_TEDS_NOTREADY)
						log(QString("- ERROR! Cannot read from TEDS, not ready (channel %1, step 7)").arg(ch));
					if (status == bu3416_TEDS_NOTPRESENT)
						log(QString("- ERROR! Cannot read from TEDS, not present (channel %1, step 7)").arg(ch));
					channelsErrorsMask |= (1 << (ch - 1));
				}
				for (i = 0; i < 2; i++) {
					if ((read_cmd[4 + i] & 0x00FF) != (write_cmd[4 + i] & 0x00FF)) {
						log(QString("- ERROR! Scratchpad read value %1 from TEDS is not equal to 0x%2 (channel %3, step 8)").arg(read_cmd[4 + i] & 0x00FF).arg(write_cmd[4 + i] & 0x00FF).arg(ch));
						log(QString("- ERROR! channel %1 failed 0x%2 0x%3").arg(ch).arg(read_cmd[4 + i] & 0x00FF).arg(write_cmd[4 + i] & 0x00FF));
						channelsErrorsMask |= (1 << (ch - 1));
					}
				}
				// 9
				connection->writePortExpander(ch, portExp);
				bu3100_sleep(50);
				// 10
				status = connection->_1wire_commander(reset_cmd);

				if (status != bu3416_TEDS_NOTPRESENT) {
					if (status == bu3416_TEDS_READY)
						log(QString("- ERROR! Cannot check TEDS, not ready (channel %1, step 10)").arg(ch));
					else if (status == VI_SUCCESS)
						log(QString("- ERROR! TEDS present but should not be (channel %1, step 10)").arg(ch));

					channelsErrorsMask |= (1 << (ch - 1));
				}
				connection->callAndThrowOnErrorT028(t028_setChannelsConfig, "t028_setChannelsConfig", 0xffff, T028_MODE_EXCAL);
				if (!(channelsErrorsMask & (1 << (ch - 1))))
					log("OK");
			}
		}
	}
	connection->writeFPGAreg(bu6716_FPGA_SEGSW, segsw_dev);
	log("SCPI/driver functionality test:");
	srand((unsigned int)time(nullptr));
	for (int i = 0; i < bu6716_NUM_CHAN; i++) {
		unsigned char data[32];
		unsigned char data_wr[32];
		if (!(CHANNEL_MASK & (1 << i)))
			continue;
		connection->callAndThrowOnErrorT028(t028_setChanConfig, "t028_setChanConfig", i + 1, T028_MODE_TEDS1);
		log(QString("CHANNEL %1:").arg(i + 1));
		for (int j = 0; j < 32; j++)
			data_wr[j] = rand();
		if (bu6716_writeTEDS(connection->getVi6716(), i + 1, bu6716_TEDS_CLASS1, 0, data_wr, 32) == VI_SUCCESS) {
			log("  EEPROM (written): ");
			for (int j = 0; j < 32; j++)
				log(QString("%1").arg(data_wr[j]));
			log("");
		}
		else {
			ViInt32 code;
			ViChar message[1024] = { 0 };
			bu6716_error_query(connection->getVi6716(), &code, message);
			log(QString("  EEPROM: ERROR! %1").arg(message));
			channelsErrorsMask |= (1 << i);
		}
		if (bu6716_readTEDS(connection->getVi6716(), i + 1, bu6716_TEDS_CLASS1, bu6716_TEDS_MEM_EEPROM, data) == VI_SUCCESS) {
			log("  EEPROM (read)   : ");
			for (int j = 0; j < 32; j++)
				log(QString("%1").arg(data[j]));
			log("");
			for (int j = 0; j < 32; j++) {
				if (data[j] != data_wr[j]) {
					log("  ERROR! Data verification failed!");
					channelsErrorsMask |= (1 << i);
					break;
				}
			}
		}
		else {
			ViInt32 code;
			ViChar message[1024] = { 0 };
			bu6716_error_query(connection->getVi6716(), &code, message);
			log(QString("  EEPROM: ERROR! %1").arg(message));
			channelsErrorsMask |= (1 << i);
		}
		if (bu6716_readTEDS(connection->getVi6716(), i + 1, bu6716_TEDS_CLASS1, bu6716_TEDS_MEM_OTPROM, data) == VI_SUCCESS) {
			log("  OTPROM          : ");
			for (int j = 0; j < 8; j++)
				log(QString("%1").arg(data[j]));
			log("");
		}
		else {
			ViInt32 code;
			ViChar message[1024] = { 0 };
			bu6716_error_query(connection->getVi6716(), &code, message);
			log(QString("  OTPROM: ERROR! %1").arg(message));
			channelsErrorsMask |= (1 << i);
		}
		if (bu6716_readTEDS(connection->getVi6716(), i + 1, bu6716_TEDS_CLASS1, bu6716_TEDS_MEM_ROM, data) == VI_SUCCESS) {
			log("  ROM             : ");
			for (int j = 0; j < 8; j++)
				log(QString("%1").arg(data[j]));
			log("");
		}
		else {
			ViInt32 code;
			ViChar message[1024] = { 0 };
			bu6716_error_query(connection->getVi6716(), &code, message);
			log(QString("  ROM:    ERROR! %1").arg(message));
			channelsErrorsMask |= (1 << i);
		}
		connection->callAndThrowOnErrorT028(t028_setChannelsConfig, "t028_setChannelsConfig", 0xffff, T028_MODE_EXCAL);
	}
	return channelsErrorsMask == 0;
}

TedsClass1Test::TedsClass1Test(const std::shared_ptr<Communication_6716>& connection) : Abstract6716Test("Teds Class 1", connection) {}