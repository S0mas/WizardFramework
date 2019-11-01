#include "../include/Tests/LEDsTest.h"
#include "../include/defines.h"

bool LEDsTest::switchLED_inTestMode(unsigned char channel, LED_STATE state) const {
	unsigned char color = (state == LED_STATE::RED) ? (2 << 2) | (3 << 0) : (3 << 2) | (2 << 0);
	auto segsw = connection->readFPGAreg(bu6716_FPGA_SEGSW);
	connection->writeFPGAreg(bu6716_FPGA_SEGSW, channel >= 8 ? 0 : 1);
	auto led = connection->readFPGAreg(bu6716_FPGA_TEST_RW);
	if (state == LED_STATE::OFF)
		led &= ~(1 << (channel % 8));
	else
		led |= (1 << (channel % 8));
	connection->writeFPGAreg(bu6716_FPGA_TEST_RW, led);
	if (state != LED_STATE::OFF)
		connection->writeFPGAreg(bu6716_FPGA_TEST_LEDS, color);

	connection->writeFPGAreg(bu6716_FPGA_SEGSW, segsw);
	return true;
}

bool LEDsTest::test() const {
	connection->callAndThrowOnError6716(bu6716_reloadConfig, "bu6716_reloadConfig", true);
	auto segsw_dev = connection->readFPGAreg(bu6716_FPGA_SEGSW);
	printf("Press enter to start the sequence...\n");
	//getchar();
	// Turn off all LEDs
	connection->writeFPGAreg(bu6716_FPGA_SEGSW, 0x1);
	connection->writeFPGAreg(bu6716_FPGA_TEST_LEDS, (2 << 2) | (2 << 0));
	connection->writeFPGAreg(bu6716_FPGA_SEGSW, 0x0);
	connection->writeFPGAreg(bu6716_FPGA_TEST_LEDS, (2 << 2) | (2 << 0));
	printf("All LEDs should be off now!\n");
	bu3100_sleep(2000);
	// Test the LEDs
	printf("Testing red LEDs...\n");
	for (int i = 0; i < 32; i++) {
		printf("CHANNEL %d\n", (i % 16) + 1);
		if (i < 16)
			switchLED_inTestMode(i % 16, LED_STATE::RED);
		else
			switchLED_inTestMode(i % 16, LED_STATE::OFF);
		bu3100_sleep(50);
	}
	printf("Testing green LEDs...\n");
	for (int i = 0; i < 32; i++) {
		printf("CHANNEL %d\n", (i % 16) + 1);
		if (i < 16)
			switchLED_inTestMode(i % 16, LED_STATE::GREEN);
		else
			switchLED_inTestMode(i % 16, LED_STATE::OFF);
		bu3100_sleep(50);
	}
	// Bring back normal operation of LEDs
	connection->writeFPGAreg(bu6716_FPGA_SEGSW, 0x1);
	connection->writeFPGAreg(bu6716_FPGA_TEST_LEDS, 0x0);
	connection->writeFPGAreg(bu6716_FPGA_SEGSW, 0x0);
	connection->writeFPGAreg(bu6716_FPGA_TEST_LEDS, 0x0);
	connection->writeFPGAreg(bu6716_FPGA_SEGSW, segsw_dev);
	return true;
}

LEDsTest::LEDsTest(const std::shared_ptr<Communication_6716>& connection) : Abstract6716Test("LEDs", connection) {}