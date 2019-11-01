#include "../include/Tests/TedsClass1Test.h"
#include "../include/defines.h"

bool TedsClass1Test::test() const {
	ViStatus status;
	int errorDetected = 0;
	//ViUInt8 segconf_dev;  // No need to change this register - TEDS class 1 is always connected to set #2
	ViUInt8 segsw_dev;
	ViUInt16 reset_cmd[] = { 0x0300, 0x0000 };
	ViUInt16 write_cmd[] = { 0x0300, 0x02CC, 0x0299, 0x0200, 0x0200, 0x0200, 0x0300, 0x0000 };
	ViUInt16 read_cmd[] = { 0x0300, 0x02CC, 0x02C3, 0x0200, 0x0100, 0x0100, 0x0300, 0x0000 };
	int i;
	connection->callAndThrowOnError6716(bu6716_reloadConfig, "bu6716_reloadConfig", true);
	connection->callAndThrowOnError6716(bu6716_setChannelConf, "bu6716_setChannelConf", CHANNEL_MASK, bu6716_MODE_FULL_BRIDGE, bu6716_GAIN_1, bu6716_COUPLING_DC, bu6716_INP_SRC_FP);
	connection->callAndThrowOnError6716(bu6716_setExcitationMonitor, "bu6716_setExcitationMonitor", bu6716_EXCMON_OFF);
	//BU6716_CHECK_ERR(readFPGAreg, connection->getVi6716(), bu6716_FPGA_SEGCONF, &segconf_dev);
	segsw_dev = connection->readFPGAreg(bu6716_FPGA_SEGSW);
	for (int j = 0; j < 3; j++) {
		printf("ITERATION %d/%d\n", j + 1, 3);
		for (int s = 0; s < 2; s++) {
			if (s == 0) {
				// 1 & 2
				connection->writeFPGAreg(bu6716_FPGA_SEGSW, 0x1);
				//BU6716_CHECK_ERR(writeFPGAreg, connection->getVi6716(), bu6716_FPGA_SEGCONF, segconf_dev & ~0x40);  // disconnect TEDS class 1 to seg #1 and #2 
			}
			else {
				// 12
				//BU6716_CHECK_ERR(writeFPGAreg, connection->getVi6716(), bu6716_FPGA_SEGSW, 0x1);
				//BU6716_CHECK_ERR(writeFPGAreg, connection->getVi6716(), bu6716_FPGA_SEGCONF, segconf_dev | 0x40);  // connect TEDS class 1 to seg #1 and #2 (possible only when SEGSW==1)
				connection->writeFPGAreg(bu6716_FPGA_SEGSW, 0x0);
			}
			bu3100_sleep(100);
			for (int c = 0; c < 8; c++) {
				unsigned short portExp = 0;
				unsigned char ch = c + 1 + s * 8;

				if (!(CHANNEL_MASK & 1 << (ch - 1)))
					continue;
				printf("CHANNEL %d\n", ch);
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
						printf("- ERROR! Cannot write to TEDS, not ready (channel: %d, step 5)\n", ch);

					if (status == bu3416_TEDS_NOTPRESENT)
						printf("- ERROR! Cannot write to TEDS, not present (channel: %d, step 5)\n", ch);

					errorDetected |= (1 << (ch - 1));
				}
				bu3100_sleep(20);
				// 7 & 8
				read_cmd[4] = 0x0100;  // reset on previous commands...
				read_cmd[5] = 0x0100;
				status = connection->_1wire_commander(read_cmd);
				if (status < VI_SUCCESS) {
					if (status == bu3416_TEDS_NOTREADY)
						printf("- ERROR! Cannot read from TEDS, not ready (channel %d, step 7)\n", ch);
					if (status == bu3416_TEDS_NOTPRESENT)
						printf("- ERROR! Cannot read from TEDS, not present (channel %d, step 7)\n", ch);
					errorDetected |= (1 << (ch - 1));
				}
				for (i = 0; i < 2; i++) {
					if ((read_cmd[4 + i] & 0x00FF) != (write_cmd[4 + i] & 0x00FF)) {
						printf("- ERROR! Scratchpad read value %02hX from TEDS is not equal to 0x%02hX (channel %d, step 8)\n", read_cmd[4 + i] & 0x00FF, write_cmd[4 + i] & 0x00FF, ch);
						printf("- ERROR! channel %d failed 0x%02hX 0x%02hX\n", ch, read_cmd[4 + i] & 0x00FF, write_cmd[4 + i] & 0x00FF);
						errorDetected |= (1 << (ch - 1));
					}
				}
				// 9
				connection->writePortExpander(ch, portExp);
				bu3100_sleep(50);
				// 10
				status = connection->_1wire_commander(reset_cmd);

				if (status != bu3416_TEDS_NOTPRESENT) {
					if (status == bu3416_TEDS_READY)
						printf("- ERROR! Cannot check TEDS, not ready (channel %d, step 10)", ch);
					else if (status == VI_SUCCESS)
						printf("- ERROR! TEDS present but should not be (channel %d, step 10)", ch);

					errorDetected |= (1 << (ch - 1));
				}
				connection->callAndThrowOnErrorT028(t028_setChannelsConfig, "t028_setChannelsConfig", 0xffff, T028_MODE_EXCAL);
				if (!(errorDetected & (1 << (ch - 1))))
					printf("  OK\n");
			}
		}
	}
	//BU6716_CHECK_ERR(writeFPGAreg, connection->getVi6716(), bu6716_FPGA_SEGCONF, segconf_dev);
	connection->writeFPGAreg(bu6716_FPGA_SEGSW, segsw_dev);
	printf("\nSCPI/driver functionality test:\n");
	srand((unsigned int)time(nullptr));
	for (int i = 0; i < bu6716_NUM_CHAN; i++) {
		unsigned char data[32];
		unsigned char data_wr[32];
		if (!(CHANNEL_MASK & (1 << i)))
			continue;
		connection->callAndThrowOnErrorT028(t028_setChanConfig, "t028_setChanConfig", i + 1, T028_MODE_TEDS1);
		printf("CHANNEL %d:\n", i + 1);
		for (int j = 0; j < 32; j++)
			data_wr[j] = rand();
		if (bu6716_writeTEDS(connection->getVi6716(), i + 1, bu6716_TEDS_CLASS1, 0, data_wr, 32) == VI_SUCCESS) {
			printf("  EEPROM (written): ");
			for (int j = 0; j < 32; j++)
				printf("%02hhx ", data_wr[j]);
			printf("\n");
		}
		else {
			ViInt32 code;
			ViChar message[1024] = { 0 };
			bu6716_error_query(connection->getVi6716(), &code, message);
			printf("  EEPROM: ERROR! %s\n", message);
			errorDetected |= (1 << i);
		}
		if (bu6716_readTEDS(connection->getVi6716(), i + 1, bu6716_TEDS_CLASS1, bu6716_TEDS_MEM_EEPROM, data) == VI_SUCCESS) {
			printf("  EEPROM (read)   : ");
			for (int j = 0; j < 32; j++)
				printf("%02hhx ", data[j]);
			printf("\n");
			for (int j = 0; j < 32; j++) {
				if (data[j] != data_wr[j]) {
					printf("  ERROR! Data verification failed!\n");
					errorDetected |= (1 << i);
					break;
				}
			}
		}
		else {
			ViInt32 code;
			ViChar message[1024] = { 0 };
			bu6716_error_query(connection->getVi6716(), &code, message);
			printf("  EEPROM: ERROR! %s\n", message);
			errorDetected |= (1 << i);
		}
		if (bu6716_readTEDS(connection->getVi6716(), i + 1, bu6716_TEDS_CLASS1, bu6716_TEDS_MEM_OTPROM, data) == VI_SUCCESS) {
			printf("  OTPROM          : ");
			for (int j = 0; j < 8; j++)
				printf("%02hhx ", data[j]);
			printf("\n");
		}
		else {
			ViInt32 code;
			ViChar message[1024] = { 0 };
			bu6716_error_query(connection->getVi6716(), &code, message);
			printf("  OTPROM: ERROR! %s\n", message);
			errorDetected |= (1 << i);
		}
		if (bu6716_readTEDS(connection->getVi6716(), i + 1, bu6716_TEDS_CLASS1, bu6716_TEDS_MEM_ROM, data) == VI_SUCCESS) {
			printf("  ROM             : ");
			for (int j = 0; j < 8; j++)
				printf("%02hhx ", data[j]);
			printf("\n");
		}
		else {
			ViInt32 code;
			ViChar message[1024] = { 0 };
			bu6716_error_query(connection->getVi6716(), &code, message);
			printf("  ROM:    ERROR! %s\n", message);
			errorDetected |= (1 << i);
		}
		connection->callAndThrowOnErrorT028(t028_setChannelsConfig, "t028_setChannelsConfig", 0xffff, T028_MODE_EXCAL);
	}
	return errorDetected == 0;
}

TedsClass1Test::TedsClass1Test(const std::shared_ptr<Communication_6716>& connection) : Abstract6716Test("Teds class 1", connection) {}