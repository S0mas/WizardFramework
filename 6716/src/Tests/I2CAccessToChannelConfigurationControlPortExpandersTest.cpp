#include "../include/Tests/I2CAccessToChannelConfigurationControlPortExpandersTest.h"

bool I2CAccessToChannelConfigurationControlPortExpandersTest::checkPortExpander() const noexcept {
	for (auto i = 0u; i < 16; ++i) {
		unsigned short expected = 1 << i;
		device6716->writePortExpander(1, expected);
		if (!checkValue(expected, device6716->readPortExpander(1), 2, 16, '0'))
			return false;
	}
	return true;
}

Result I2CAccessToChannelConfigurationControlPortExpandersTest::test() const {
	auto const& portExp1Status = testWriteRead(1, bu6716_FPGA_SEGSW) && checkPortExpander();
	auto const& portExp2Status = testWriteRead(0, bu6716_FPGA_SEGSW) && checkPortExpander();
	return (portExp1Status && portExp2Status) ? Result::VALUE::PASSED : Result::VALUE::FAILED;
}

I2CAccessToChannelConfigurationControlPortExpandersTest::I2CAccessToChannelConfigurationControlPortExpandersTest()
	: AbstractTest6716("I2C Access to channel configuration control port expanders") {}