#include "../../include/Tests/I2CAccessToFPGATest.h"

bool I2CAccessToFPGATest::testRWAndCheckId(const unsigned char expectedValue) const noexcept {
	device6716->writeFPGAreg(bu6716_FPGA_TEST_RW, expectedValue);
	if (!checkId())
		return false;
	auto read = device6716->readFPGAreg(bu6716_FPGA_TEST_RW);
	return checkValue(expectedValue, read, 2, 8, '0');
}

Result I2CAccessToFPGATest::test() const {
	if (!checkId())
		return Result::VALUE::FAILED;
	for (int i = 0; i < 8; ++i)
		if (!testRWAndCheckId(~(1 << i)))
			return Result::VALUE::FAILED;
	for (int i = 0; i < 8; ++i)
		if (!testRWAndCheckId(1 << i))
			return Result::VALUE::FAILED;
	return Result::VALUE::PASSED;
}

I2CAccessToFPGATest::I2CAccessToFPGATest() : AbstractTest6716("I2C Access To FPGA") {}
