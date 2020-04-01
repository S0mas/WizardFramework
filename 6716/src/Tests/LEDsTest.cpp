#include "../include/Tests/LEDsTest.h"

bool LEDsTest::switchLED_inTestMode(const unsigned char channel, const LED_STATE state) const {
	unsigned char color = (state == LED_STATE::RED) ? (2 << 2) | (3 << 0) : (3 << 2) | (2 << 0);
	auto segsw = device6716->readFPGAreg(bu6716_FPGA_SEGSW);
	device6716->writeFPGAreg(bu6716_FPGA_SEGSW, channel >= 8 ? 0 : 1);
	auto led = device6716->readFPGAreg(bu6716_FPGA_TEST_RW);
	if (state == LED_STATE::OFF)
		led &= ~(1 << (channel % 8));
	else
		led |= (1 << (channel % 8));
	device6716->writeFPGAreg(bu6716_FPGA_TEST_RW, led);
	if (state != LED_STATE::OFF)
		device6716->writeFPGAreg(bu6716_FPGA_TEST_LEDS, color);

	device6716->writeFPGAreg(bu6716_FPGA_SEGSW, segsw);
	return true;
}

void LEDsTest::testLED(const LED_STATE state) const {
	QString stateString = state == LED_STATE::GREEN ? "GREEN" : state == LED_STATE::RED ? "RED" : "OFF";
	for (auto& channel : device6716->channels()) {
		if (channel.disabled())
			continue;
		log(QString("CHANNEL %1 --> %2").arg(channel.index()).arg(stateString));
		switchLED_inTestMode(channel.index() - 1, state);
		bu3100_sleep(500);
	}
}

void LEDsTest::logAboutMarkedFailedChannels(const std::vector<bool>& channels) const noexcept {
	bool errorReported = false;
	for (int index = 0; index < channels.size(); ++index)
		if (channels[index]) {
			errorReported = true;
			log(QString("User reported error for channel %1").arg(index + 1));
		}
	if (!errorReported)
		log("No errors reported by user.");
}

Result LEDsTest::test() const {
	// 1
	device6716->writeFPGAreg(bu6716_FPGA_SEGSW, 0x1);

	// 2
	device6716->writeFPGAreg(bu6716_FPGA_TEST_LEDS, 0x0A);
	
	// 3
	device6716->writeFPGAreg(bu6716_FPGA_SEGSW, 0x0);

	// 4
	device6716->writeFPGAreg(bu6716_FPGA_TEST_LEDS, 0x0A);

	for (auto& channel : device6716->channels())
		if (channel.enabled())
			channelsResults.at(channel.index()) = Result::VALUE::PASSED;

	// 5
	sender_->waitForConfirmation("All LEDs should be off now!");

	// 6 & 7 & 8 & 9 & 10 & 11
	sender_->waitForConfirmation("The selected channels LEDs will lights up RED and then turn off in order.");
	testLED(LED_STATE::RED);

	// 13 & 14 & 15
	testLED(LED_STATE::OFF);

	// 12
	auto selectedChanels = sender_->waitForChannelsSelection("Please, select all channels with faulty leds and click Ok", device6716->channelsStates());
	for (int i = 0; i < selectedChanels.size(); ++i)
		channelsResults.at(i + 1) = Result::VALUE::FAILED;
	logAboutMarkedFailedChannels(selectedChanels);

	//16
	sender_->waitForConfirmation("The selected channels LEDs will lights up GREEN and then turn off in order.");
	testLED(LED_STATE::GREEN);
	testLED(LED_STATE::OFF);
	selectedChanels = sender_->waitForChannelsSelection("Please, select all channels with faulty leds and click Ok", device6716->channelsStates());
	for (int i = 0; i < selectedChanels.size(); ++i)
		channelsResults.at(i + 1) = Result::VALUE::FAILED;
	logAboutMarkedFailedChannels(selectedChanels);

	return channelsResult();
}

LEDsTest::LEDsTest() : AbstractTest6716("LEDs") {}
