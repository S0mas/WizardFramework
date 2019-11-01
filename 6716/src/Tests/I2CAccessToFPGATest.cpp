#include "../include/Tests/I2CAccessToFPGATest.h"
#include "../include/defines.h"

bool I2CAccessToFPGATest::testRWAndCheckId(const unsigned char expectedValue) const noexcept {
	connection->writeFPGAreg(bu6716_FPGA_TEST_RW, expectedValue);
	if (!checkId())
		return false;
	auto read = connection->readFPGAreg(bu6716_FPGA_TEST_RW);
	return checkValue(expectedValue, read, 2, 8, '0');
}

bool I2CAccessToFPGATest::test() const {
	if (!checkId())
		return false;
	for (int i = 0; i < 8; ++i)
		if (!testRWAndCheckId(~(1 << i)))
			return false;
	for (int i = 0; i < 8; ++i)
		if (!testRWAndCheckId(1 << i))
			return false;
	return true;
}

I2CAccessToFPGATest::I2CAccessToFPGATest(const std::shared_ptr<Communication_6716>& connection) : Abstract6716Test("I2C Access To FPGA", connection) {}
