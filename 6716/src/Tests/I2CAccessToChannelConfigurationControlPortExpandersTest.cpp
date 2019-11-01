#include "../include/Tests/I2CAccessToChannelConfigurationControlPortExpandersTest.h"
#include "../include/defines.h"

bool I2CAccessToChannelConfigurationControlPortExpandersTest::checkPortExpander() const noexcept {
	for (auto i = 0u; i < 16; ++i) {
		unsigned short expected = 1 << i;
		connection->writePortExpander(1, expected);
		if (!checkValue(expected, connection->readPortExpander(1), 2, 16, '0'))
			return false;
	}
	return true;
}

bool I2CAccessToChannelConfigurationControlPortExpandersTest::test() const {
	auto const& portExp1Status = testWriteRead(1, bu6716_FPGA_SEGSW) && checkPortExpander();
	auto const& portExp2Status = testWriteRead(0, bu6716_FPGA_SEGSW) && checkPortExpander();
	return portExp1Status && portExp2Status;
}

I2CAccessToChannelConfigurationControlPortExpandersTest::I2CAccessToChannelConfigurationControlPortExpandersTest(const std::shared_ptr<Communication_6716>& connection)
	: Abstract6716Test("I2C Access to channel configuration control port expanders", connection) {}